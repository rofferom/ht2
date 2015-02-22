#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <histedit.h>
#include <ht-lua.hpp>
#include <ht-legacy-lua.hpp>

extern "C" {
	#include <lualib.h>
}

#define XSTR(a) STR(a)
#define STR(a) #a

struct Params {
	bool interactive = false;
	bool dump = false;
	bool verbose = false;
	bool version = false;
};

static const struct option longOptions[] = {
	{"interactive", no_argument, NULL, 'i'},
	{"dump", no_argument, NULL, 'd'},
	{"verbose", no_argument, NULL, 'v'},
	{"version", no_argument, NULL, 'V'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0},
};

struct Prompt {
	EditLine *el = NULL;
	History *history = NULL;
	HistEvent histEvt;
};

static void printUsage(const char *prog)
{
	printf(
		"Usage : %s [OPTION]... [LUA FILE]...\n"
		"Options :\n"
		"\t-i, --interactive\tStart Lua prompt\n"
		"\t-d, --dump\t\tDump Lua API\n"
		"\t-v, --verbose\t\tIncrease log level\n"
		"\t-V  --version\t\tShow version\n"
		"\t-h, --help\t\tShow this help\n",
		prog
	);
}

static void printVersion()
{
	char strBuildDate[128];
	time_t buildDate = BUILDDATE;
	struct tm tm;

	localtime_r(&buildDate, &tm);
	strftime(strBuildDate, sizeof(strBuildDate), "%F %H:%M:%S", &tm);

	printf("Version : %s\n", XSTR(REVISION));
	printf("Buildtime : %s\n", strBuildDate);
}

static int parseParams(Params *params, int argc, char *argv[])
{
	int res;
	int optionIndex = 0;
	int c;

	res = 0;
	while ((c = getopt_long(argc, argv, "ivhVd", longOptions, &optionIndex)) != -1) {
		switch (c) {
		case 'i':
			params->interactive = true;
			break;

		case 'v':
			params->verbose = true;
			break;

		case 'V':
			params->version = true;
			break;

		case 'd':
			params->dump = true;
			break;

		case 'h':
		default:
			res = -EINVAL;
			break;
		}

		if (res < 0) {
			break;
		}
	}

	if (res == 0) {
		res = optind;
	}

	return res;
}

static const char *promptCb(EditLine *e)
{
	return "> ";
}

static int initPrompt(Prompt *prompt, const char *prog)
{
	int res = 0;

	// Init prompt
	prompt->el = el_init(prog, stdin, stdout, stderr);
	if (prompt->el == NULL) {
		res = ENOMEM;
		goto error;
	}

	el_set(prompt->el, EL_PROMPT, promptCb);
	el_set(prompt->el, EL_EDITOR, "emacs");

	prompt->history = history_init();
	if (prompt->history == NULL) {
		res = ENOMEM;
		goto error;
	}

	res = history(prompt->history, &prompt->histEvt, H_SETSIZE, 20);
	if (res < 0) {
		goto error;
	}

	res = el_set(prompt->el, EL_HIST, history, prompt->history);
	if (res < 0) {
		goto error;
	}

error:
	return res;
}

static void clearPrompt(Prompt *prompt)
{
	// Close prompt
	if (prompt->history) {
		history_end(prompt->history);
		prompt->history = NULL;
	}

	if (prompt->el) {
		el_end(prompt->el);
		prompt->el = NULL;
	}
}

static void runPrompt(lua_State *L, Prompt *prompt)
{
	int res;

	while (true) {
		const char *line;
		int lineSize;

		line = el_gets(prompt->el, &lineSize);
		if (line == NULL) {
			break;
		} else if (line[0] == '\n') {
			continue;
		}

		history(prompt->history, &prompt->histEvt, H_ENTER, line);

		res = luaL_loadbuffer(L, line, lineSize, "input");
		if (res != LUA_OK) {
			printf("%s\n", lua_tostring(L, -1));
			continue;
		}

		res = lua_pcall(L, 0, LUA_MULTRET, 0);
		if (res != LUA_OK) {
			printf("%s\n", lua_tostring(L, -1));
		}
	}
}

static void registerBuiltinComponents(lua_State *L)
{
	htlua::registerComponents(L);
	htlual::registerComponents(L);
}

static void printBuiltinComponents()
{
	htlua::printComponents();
	htlual::printComponents();
}

int main(int argc, char *argv[])
{
	lua_State *L = NULL;
	Prompt prompt;
	Params params;
	int res = 0;
	int nextArg;

	if (argc == 1) {
		printUsage(argv[0]);
		return 1;
	}

	// Parse args
	res = parseParams(&params, argc, argv);
	if (res < 0) {
		printUsage(argv[0]);
		return 1;
	}

	nextArg = res;

	if (params.version) {
		printVersion();
		return 0;
	}

	// Forbid non-interactive mode without Lua files to execute
	if (nextArg == argc && params.interactive == false && params.dump == false) {
		printUsage(argv[0]);
		return 1;
	}

	// Enable verbose if required
	if (params.verbose) {
		ht::Log::setLogLevel(ht::Log::Level::Debug);
	}

	// Init Lua
	L = luaL_newstate();
	if (L == NULL) {
		res = ENOMEM;
		goto error;
	}

	luaL_openlibs(L);
	registerBuiltinComponents(L);

	if (params.dump) {
		printBuiltinComponents();
		return 0;
	}

	// Execute lua files
	if (nextArg < argc) {
		for (int i = nextArg ; i < argc ; i++) {
			res = luaL_loadfile(L, argv[i]);
			if (res != LUA_OK) {
				printf("%s\n", lua_tostring(L, -1));
				break;
			}

			res = lua_pcall(L, 0, LUA_MULTRET, 0);
			if (res != LUA_OK) {
				printf("%s\n", lua_tostring(L, -1));
			}
		}

		if (res != LUA_OK) {
			goto error;
		}
	}

	// Start prompt if required
	if (params.interactive) {

		res = initPrompt(&prompt, argv[0]);
		if (res < 0) {
			goto error;
		}

		runPrompt(L, &prompt);
	}

error:
	clearPrompt(&prompt);

	// Close Lua
	if (L) {
		lua_close(L);
	}

	return res;
}
