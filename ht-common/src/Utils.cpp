#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <ht/Log.hpp>
#include <ht/Utils.hpp>

static const char *TAG = "Utils";

namespace ht {

int loadFile(const char *path, char **outContent, size_t *outContentSize)
{
	FILE *f = NULL;
	char *content = NULL;
	size_t contentSize;
	int res;
	struct stat pathStat;

	if (outContent == NULL || outContentSize == NULL) {
		return -EINVAL;
	}

	// Load file content
	res = stat(path, &pathStat);
	if (res == -1) {
		Log::d(TAG, "Unable to open file '%s' : %s", path, strerror(errno));
		res = -errno;
		goto error;
	}

	contentSize = pathStat.st_size;
	content = (char *) malloc(contentSize);
	if (content == NULL) {
		res = -ENOMEM;
		goto error;
	}

	f = fopen(path, "rb");
	if (f == NULL) {
		Log::d(TAG, "Unable to open file '%s' : %s", path, strerror(errno));
		res = -errno;
		goto error;
	}

	res = fread(content, 1, contentSize, f);
	if ((size_t) res != contentSize) {
		Log::d(TAG, "Error while reading file '%s' : %s", path, strerror(errno));
		res = -errno;
		goto error;
	}

	fclose(f);

	*outContent = content;
	*outContentSize = contentSize;

	return res;

error:
	if (f != NULL) {
		fclose(f);
	}

	free(content);

	return res;
}

} // ht
