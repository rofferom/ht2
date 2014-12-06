#ifndef __HT_TEXTFILE_HPP__
#define __HT_TEXTFILE_HPP__

#include <stdlib.h>

namespace ht {

class TextFile {
public:
	static int load(
		const char *path,
		const char *encoding,
		char32_t **outContent,
		size_t *outSize);

	static void clear(char32_t *content);
};

} // ht

#endif // !__HT_TEXTFILE_HPP__
