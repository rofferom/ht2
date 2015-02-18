#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "ht-private.hpp"

static const char32_t *TAG = U"Utils";

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
		Log::w(
			TAG, U"Unable to open file '%s' : %s",
			SysEnv::toInternal(path).c_str(), SysEnv::toInternal(strerror(errno)).c_str());

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
		Log::w(
			TAG, U"Unable to open file '%s' : %s",
			SysEnv::toInternal(path).c_str(), SysEnv::toInternal(strerror(errno)).c_str());

		res = -errno;
		goto error;
	}

	res = fread(content, 1, contentSize, f);
	if ((size_t) res != contentSize) {
		Log::w(
			TAG, U"Error while reading file '%s' : %s",
			SysEnv::toInternal(strerror(errno)).c_str());

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

int dumpToFile(const char *path, const uint8_t *buffer, size_t size)
{
	FILE *f = NULL;
	int res;

	f = fopen(path, "wb");
	if (f == NULL) {
		res = -EINVAL;
		goto error;
	}

	fwrite(buffer, 1, size, f);

	fclose(f);

	return 0;

error:
	if (f != NULL) {
		fclose(f);
	}

	return res;
}

int compareFiles(const char *path1, const char *path2)
{
	char *file1Content = NULL;
	size_t file1ContentSize;
	char *file2Content = NULL;
	size_t file2ContentSize;
	int res;

	res = loadFile(path1, &file1Content, &file1ContentSize);
	if (res < 0) {
		goto error;
	}

	res = loadFile(path2, &file2Content, &file2ContentSize);
	if (res < 0) {
		goto error;
	}

	if (file1ContentSize != file2ContentSize) {
		res = -EINVAL;
		goto error;
	}

	if (memcmp(file1Content, file2Content, file1ContentSize) != 0) {
		res = -EINVAL;
		goto error;
	}

	free(file1Content);
	free(file2Content);

	return 0;

error:
	free(file1Content);
	free(file2Content);
	return res;
}

} // ht
