#ifndef __HTL_TEXT_HPP__
#define __HTL_TEXT_HPP__

#include <ht/Text.hpp>

namespace htl {

int loadText(const char *textPath, const char *encoding, ht::Text *text);
int saveText(const ht::Text *text, const char *textPath, const char *encoding);

} // namespace htl

#endif // !__HTL_TEXT_HPP__
