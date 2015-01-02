#ifndef __HTL_TEXT_TOKENIZER_HPP__
#define __HTL_TEXT_TOKENIZER_HPP__

#include <stdint.h>

namespace htl {

struct TokenizerCb {
	void (*pointerFound)(uint32_t id, void *userdata);
	void (*rawByteFound)(uint8_t byte, void *userdata);
	void (*textFound)(const char32_t *s, size_t size, void *userdata);
	void *userdata;
};

int tokenizeText(const char *textPath, const char *encoding, const TokenizerCb &cb);

} // namespace htl

#endif // !__HTL_TEXT_TOKENIZER_HPP__
