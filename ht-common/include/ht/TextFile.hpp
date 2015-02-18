#ifndef __HT_TEXTFILE_HPP__
#define __HT_TEXTFILE_HPP__

namespace ht {

class TextFile {
public:
	HTAPI static int load(
		const char *path,
		const char *encoding,
		char32_t **outContent,
		size_t *outSize);

	HTAPI static void clear(char32_t *content);
};

} // ht

#endif // !__HT_TEXTFILE_HPP__
