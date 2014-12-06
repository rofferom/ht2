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
 * Header for int64.c.
 */

#ifndef PUTTY_INT64_H
#define PUTTY_INT64_H

typedef struct {
    unsigned long hi, lo;
} uint64;

uint64 uint64_div10(uint64 x, int *remainder);
void uint64_decimal(uint64 x, char *buffer);
uint64 uint64_make(unsigned long hi, unsigned long lo);
uint64 uint64_add(uint64 x, uint64 y);
uint64 uint64_add32(uint64 x, unsigned long y);
int uint64_compare(uint64 x, uint64 y);
uint64 uint64_subtract(uint64 x, uint64 y);
double uint64_to_double(uint64 x);
uint64 uint64_shift_right(uint64 x, int shift);
uint64 uint64_shift_left(uint64 x, int shift);
uint64 uint64_from_decimal(char *str);

#endif
