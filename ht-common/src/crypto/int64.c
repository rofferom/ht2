/*
 * PuTTY is copyright 1997-2013 Simon Tatham.
 *
 * Portions copyright Robert de Bath, Joris van Rantwijk, Delian
 * Delchev, Andreas Schultz, Jeroen Massar, Wez Furlong, Nicolas Barry,
 * Justin Bradford, Ben Harris, Malcolm Smith, Ahmad Khalifa, Markus
 * Kuhn, Colin Watson, and CORE SDI S.A.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Handling of the int64 and uint64 types. Done in 32-bit integers,
 * for (pre-C99) portability. Hopefully once C99 becomes widespread
 * we can kiss this lot goodbye...
 */

#include <assert.h>
#include <string.h>

#include "crypto/int64.h"

uint64 uint64_div10(uint64 x, int *remainder)
{
    uint64 y;
    unsigned int rem, r2;
    y.hi = x.hi / 10;
    y.lo = x.lo / 10;
    rem = x.lo % 10;
    /*
     * Now we have to add in the remainder left over from x.hi.
     */
    r2 = x.hi % 10;
    y.lo += r2 * 429496729;
    rem += r2 * 6;
    y.lo += rem / 10;
    rem %= 10;

    if (remainder)
	*remainder = rem;
    return y;
}

void uint64_decimal(uint64 x, char *buffer)
{
    char buf[20];
    int start = 20;
    int d;

    do {
	x = uint64_div10(x, &d);
	assert(start > 0);
	buf[--start] = d + '0';
    } while (x.hi || x.lo);

    memcpy(buffer, buf + start, sizeof(buf) - start);
    buffer[sizeof(buf) - start] = '\0';
}

uint64 uint64_make(unsigned long hi, unsigned long lo)
{
    uint64 y;
    y.hi = hi & 0xFFFFFFFFU;
    y.lo = lo & 0xFFFFFFFFU;
    return y;
}

uint64 uint64_add(uint64 x, uint64 y)
{
    x.lo = (x.lo + y.lo) & 0xFFFFFFFFU;
    x.hi += y.hi + (x.lo < y.lo ? 1 : 0);
    return x;
}

uint64 uint64_add32(uint64 x, unsigned long y)
{
    uint64 yy;
    yy.hi = 0;
    yy.lo = y;
    return uint64_add(x, yy);
}

int uint64_compare(uint64 x, uint64 y)
{
    if (x.hi != y.hi)
	return x.hi < y.hi ? -1 : +1;
    if (x.lo != y.lo)
	return x.lo < y.lo ? -1 : +1;
    return 0;
}

uint64 uint64_subtract(uint64 x, uint64 y)
{
    x.lo = (x.lo - y.lo) & 0xFFFFFFFFU;
    x.hi = (x.hi - y.hi - (x.lo > (y.lo ^ 0xFFFFFFFFU) ? 1 : 0)) & 0xFFFFFFFFU;
    return x;
}

double uint64_to_double(uint64 x)
{
    return (4294967296.0 * x.hi) + (double)x.lo;
}

uint64 uint64_shift_right(uint64 x, int shift)
{
    if (shift < 32) {
	x.lo >>= shift;
	x.lo |= (x.hi << (32-shift)) & 0xFFFFFFFFU;
	x.hi >>= shift;
    } else {
	x.lo = x.hi >> (shift-32);
	x.hi = 0;
    }
    return x;
}

uint64 uint64_shift_left(uint64 x, int shift)
{
    if (shift < 32) {
	x.hi = (x.hi << shift) & 0xFFFFFFFFU;
	x.hi |= (x.lo >> (32-shift));
	x.lo = (x.lo << shift) & 0xFFFFFFFFU;
    } else {
	x.hi = (x.lo << (shift-32)) & 0xFFFFFFFFU;
	x.lo = 0;
    }
    return x;
}

uint64 uint64_from_decimal(char *str)
{
    uint64 ret;
    ret.hi = ret.lo = 0;
    while (*str >= '0' && *str <= '9') {
	ret = uint64_add(uint64_shift_left(ret, 3),
			 uint64_shift_left(ret, 1));
	ret = uint64_add32(ret, *str - '0');
	str++;
    }
    return ret;
}

#ifdef TESTMODE

#include <stdio.h>

int main(void)
{
    uint64 x, y, z;
    char buf[80];

    x = uint64_make(0x3456789AUL, 0xDEF01234UL);
    printf("%08lx.%08lx\n", x.hi, x.lo);
    uint64_decimal(x, buf);
    printf("%s\n", buf);

    y = uint64_add32(x, 0xFFFFFFFFU);
    printf("%08lx.%08lx\n", y.hi, y.lo);
    uint64_decimal(y, buf);
    printf("%s\n", buf);

    z = uint64_subtract(y, x);
    printf("%08lx.%08lx\n", z.hi, z.lo);
    uint64_decimal(z, buf);
    printf("%s\n", buf);

    z = uint64_subtract(x, y);
    printf("%08lx.%08lx\n", z.hi, z.lo);
    uint64_decimal(z, buf);
    printf("%s\n", buf);

    y = uint64_shift_right(x, 4);
    printf("%08lx.%08lx\n", y.hi, y.lo);

    y = uint64_shift_right(x, 36);
    printf("%08lx.%08lx\n", y.hi, y.lo);

    y = uint64_shift_left(x, 4);
    printf("%08lx.%08lx\n", x.hi, x.lo);

    y = uint64_shift_left(x, 36);
    printf("%08lx.%08lx\n", x.hi, x.lo);

    return 0;
}
#endif
