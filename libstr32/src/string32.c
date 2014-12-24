#include <stdio.h>
#include <string32.h>

char32_t *memchr32(char32_t *s, char32_t c, size_t n)
{
	char32_t *res = NULL;
	size_t i;

	for (i = 0 ; i < n ; i++) {
		if (s[i] == c) {
			res = &s[i];
			break;
		}
	}

	return res;
}

