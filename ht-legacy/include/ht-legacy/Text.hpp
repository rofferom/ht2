#ifndef __HTL_TEXT_HPP__
#define __HTL_TEXT_HPP__

#include <ht/Text.hpp>
#include <ht/Env.hpp>

namespace htl {

HTAPI int loadText(const char *textPath, const char *encoding, ht::Text *text);
HTAPI int saveText(const ht::Text *text, const char *textPath, const char *encoding);

} // namespace htl

#endif // !__HTL_TEXT_HPP__
