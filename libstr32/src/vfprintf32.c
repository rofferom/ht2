#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <inttypes.h>
#include <math.h>
#include <float.h>
#include <string32.h>
#include <vfprintf32.h>

/* Some useful macros */

#define NL_ARGMAX 9
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))

/* Convenient bit representation for modifier flags, which all fall
 * within 31 codepoints of the space character. */

#define ALT_FORM   (1U<<(U'#'-U' '))
#define ZERO_PAD   (1U<<(U'0'-U' '))
#define LEFT_ADJ   (1U<<(U'-'-U' '))
#define PAD_POS    (1U<<(U' '-U' '))
#define MARK_POS   (1U<<(U'+'-U' '))
#define GROUPED    (1U<<(U'\''-U' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|LEFT_ADJ|PAD_POS|MARK_POS|GROUPED)

#if UINT_MAX == ULONG_MAX
#define LONG_IS_INT
#endif

#if SIZE_MAX != ULONG_MAX || UINTMAX_MAX != ULLONG_MAX
#define ODD_TYPES
#endif

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success. */

enum {
	BARE, LPRE, LLPRE, HPRE, HHPRE, BIGLPRE,
	ZTPRE, JPRE,
	STOP,
	PTR, INT, UINT, ULLONG,
#ifndef LONG_IS_INT
	LONG, ULONG,
#else
#define LONG INT
#define ULONG UINT
#endif
	SHORT, USHORT, CHAR,
#ifdef ODD_TYPES
	LLONG, SIZET, IMAX, UMAX, PDIFF, UIPTR,
#else
#define LLONG ULLONG
#define SIZET ULONG
#define IMAX LLONG
#define UMAX ULLONG
#define PDIFF LONG
#define UIPTR ULONG
#endif
	DBL, LDBL,
	NOARG,
	MAXSTATE
};

#define S(x) [(x)-U'A']

static const unsigned char states[][U'z'-U'A'+1] = {
	{ /* 0: bare types */
		S(U'd') = INT, S(U'i') = INT,
		S(U'o') = UINT, S(U'u') = UINT, S(U'x') = UINT, S(U'X') = UINT,
		S(U'e') = DBL, S(U'f') = DBL, S(U'g') = DBL, S(U'a') = DBL,
		S(U'E') = DBL, S(U'F') = DBL, S(U'G') = DBL, S(U'A') = DBL,
		S(U'c') = CHAR, S(U'C') = INT,
		S(U's') = PTR, S(U'S') = PTR, S(U'p') = UIPTR, S(U'n') = PTR,
		S(U'm') = NOARG,
		S(U'l') = LPRE, S(U'h') = HPRE, S(U'L') = BIGLPRE,
		S(U'z') = ZTPRE, S(U'j') = JPRE, S(U't') = ZTPRE,
	}, { /* 1: l-prefixed */
		S(U'd') = LONG, S(U'i') = LONG,
		S(U'o') = ULONG, S(U'u') = ULONG, S(U'x') = ULONG, S(U'X') = ULONG,
		S(U'e') = DBL, S(U'f') = DBL, S(U'g') = DBL, S(U'a') = DBL,
		S(U'E') = DBL, S(U'F') = DBL, S(U'G') = DBL, S(U'A') = DBL,
		S(U'c') = INT, S(U's') = PTR, S(U'n') = PTR,
		S(U'l') = LLPRE,
	}, { /* 2: ll-prefixed */
		S(U'd') = LLONG, S(U'i') = LLONG,
		S(U'o') = ULLONG, S(U'u') = ULLONG,
		S(U'x') = ULLONG, S(U'X') = ULLONG,
		S(U'n') = PTR,
	}, { /* 3: h-prefixed */
		S(U'd') = SHORT, S(U'i') = SHORT,
		S(U'o') = USHORT, S(U'u') = USHORT,
		S(U'x') = USHORT, S(U'X') = USHORT,
		S(U'n') = PTR,
		S(U'h') = HHPRE,
	}, { /* 4: hh-prefixed */
		S(U'd') = CHAR, S(U'i') = CHAR,
		S(U'o') = CHAR, S(U'u') = CHAR,
		S(U'x') = CHAR, S(U'X') = CHAR,
		S(U'n') = PTR,
	}, { /* 5: L-prefixed */
		S(U'e') = LDBL, S(U'f') = LDBL, S(U'g') = LDBL, S(U'a') = LDBL,
		S(U'E') = LDBL, S(U'F') = LDBL, S(U'G') = LDBL, S(U'A') = LDBL,
		S(U'n') = PTR,
	}, { /* 6: z- or t-prefixed (assumed to be same size) */
		S(U'd') = PDIFF, S(U'i') = PDIFF,
		S(U'o') = SIZET, S(U'u') = SIZET,
		S(U'x') = SIZET, S(U'X') = SIZET,
		S(U'n') = PTR,
	}, { /* 7: j-prefixed */
		S(U'd') = IMAX, S(U'i') = IMAX,
		S(U'o') = UMAX, S(U'u') = UMAX,
		S(U'x') = UMAX, S(U'X') = UMAX,
		S(U'n') = PTR,
	}
};

#define OOB(x) ((unsigned)(x)-U'A' > U'z'-U'A')

union arg
{
	uintmax_t i;
	long double f;
	void *p;
};

static void pop_arg(union arg *arg, int type, va_list *ap)
{
	/* Give the compiler a hint for optimizing the switch. */
	if ((unsigned)type > MAXSTATE) return;
	switch (type) {
	       case PTR:	arg->p = va_arg(*ap, void *);
	break; case INT:	arg->i = va_arg(*ap, int);
	break; case UINT:	arg->i = va_arg(*ap, unsigned int);
#ifndef LONG_IS_INT
	break; case LONG:	arg->i = va_arg(*ap, long);
	break; case ULONG:	arg->i = va_arg(*ap, unsigned long);
#endif
	break; case ULLONG:	arg->i = va_arg(*ap, unsigned long long);
	break; case SHORT:	arg->i = (short)va_arg(*ap, int);
	break; case USHORT:	arg->i = (unsigned short)va_arg(*ap, int);
	break; case CHAR:	arg->i = (char32_t)va_arg(*ap, int);
#ifdef ODD_TYPES
	break; case LLONG:	arg->i = va_arg(*ap, long long);
	break; case SIZET:	arg->i = va_arg(*ap, size_t);
	break; case IMAX:	arg->i = va_arg(*ap, intmax_t);
	break; case UMAX:	arg->i = va_arg(*ap, uintmax_t);
	break; case PDIFF:	arg->i = va_arg(*ap, ptrdiff_t);
	break; case UIPTR:	arg->i = (uintptr_t)va_arg(*ap, void *);
#endif
	break; case DBL:	arg->f = va_arg(*ap, double);
	break; case LDBL:	arg->f = va_arg(*ap, long double);
	}
}

static void out(struct OutputStream *f, const char32_t *s, size_t l)
{
	f->write(s, l, f->userdata);
}

static void pad(struct OutputStream *f, char32_t c, size_t w, size_t l, int fl)
{
	char32_t buff[256];
	size_t count;
	size_t i;

	if ((fl & (LEFT_ADJ | ZERO_PAD)) || (l >= w)) return;
	l = w - l;

	count = l>sizeof buff ? sizeof buff : l;
	for (i = 0 ; i < count ; i++) {
		buff[i] = c;
	}

	for (; l >= sizeof buff; l -= sizeof buff)
		out(f, buff, sizeof buff);
	out(f, buff, l);
}

static const char32_t xdigits[16] = {
	U"0123456789ABCDEF"
};

static char32_t *fmt_x(uintmax_t x, char32_t *s, int lower)
{
	for (; x; x>>=4) *--s = xdigits[(x&15)]|lower;
	return s;
}

static char32_t *fmt_o(uintmax_t x, char32_t *s)
{
	for (; x; x>>=3) *--s = U'0' + (x&7);
	return s;
}

static char32_t *fmt_u(uintmax_t x, char32_t *s)
{
	unsigned long y;
	for (   ; x>ULONG_MAX; x/=10) *--s = U'0' + x%10;
	for (y=x;           y; y/=10) *--s = U'0' + y%10;
	return s;
}

/* Do not override this check. The floating point printing code below
 * depends on the float.h constants being right. If they are wrong, it
 * may overflow the stack. */
#if LDBL_MANT_DIG == 53
typedef char compiler_defines_long_double_incorrectly[9-(int)sizeof(long double)];
#endif

static int fmt_fp(struct OutputStream *f, long double y, int w, int p, int fl, int t)
{
	uint32_t big[(LDBL_MANT_DIG+28)/29 + 1          // mantissa expansion
		+ (LDBL_MAX_EXP+LDBL_MANT_DIG+28+8)/9]; // exponent expansion
	uint32_t *a, *d, *r, *z;
	int e2=0, e, j, l;
	uint32_t i;
	char32_t buf[9+LDBL_MANT_DIG/4], *bufit;
	const char32_t *prefix=U"-0X+0X 0X-0x+0x 0x";
	int pl;
	char32_t ebuf0[3*sizeof(int)], *ebuf=&ebuf0[3*sizeof(int)], *estr;

	pl=1;
	if (signbit(y)) {
		y=-y;
	} else if (fl & MARK_POS) {
		prefix+=3;
	} else if (fl & PAD_POS) {
		prefix+=6;
	} else prefix++, pl=0;

	if (!isfinite(y)) {
		const char32_t *s = (t&32)?U"inf":U"INF";
		if (y!=y) s=(t&32)?U"nan":U"NAN", pl=0;
		pad(f, U' ', w, 3+pl, fl&~ZERO_PAD);
		out(f, prefix, pl);
		out(f, s, 3);
		pad(f, U' ', w, 3+pl, fl^LEFT_ADJ);
		return MAX(w, 3+pl);
	}

	y = frexpl(y, &e2) * 2;
	if (y) e2--;

	if ((t|32)==U'a') {
		long double round = 8.0;
		int re;

		if (t&32) prefix += 9;
		pl += 2;

		if (p<0 || p>=LDBL_MANT_DIG/4-1) re=0;
		else re=LDBL_MANT_DIG/4-1-p;

		if (re) {
			while (re--) round*=16;
			if (*prefix=='-') {
				y=-y;
				y-=round;
				y+=round;
				y=-y;
			} else {
				y+=round;
				y-=round;
			}
		}

		estr=fmt_u(e2<0 ? -e2 : e2, ebuf);
		if (estr==ebuf) *--estr='0';
		*--estr = (e2<0 ? U'-' : U'+');
		*--estr = t+(U'p'-U'a');

		bufit=buf;
		do {
			int x=y;
			*bufit++=xdigits[x]|(t&32);
			y=16*(y-x);
			if (bufit-buf==1 && (y||p>0||(fl&ALT_FORM))) *bufit++='.';
		} while (y);

		if (p && bufit-buf-2 < p)
			l = (p+2) + (ebuf-estr);
		else
			l = (bufit-buf) + (ebuf-estr);

		pad(f, U' ', w, pl+l, fl);
		out(f, prefix, pl);
		pad(f, U'0', w, pl+l, fl^ZERO_PAD);
		out(f, buf, bufit-buf);
		pad(f, U'0', l-(ebuf-estr)-(bufit-buf), 0, 0);
		out(f, estr, ebuf-estr);
		pad(f, U' ', w, pl+l, fl^LEFT_ADJ);
		return MAX(w, pl+l);
	}
	if (p<0) p=6;

	if (y) y *= 0x1p28, e2-=28;

	if (e2<0) a=r=z=big;
	else a=r=z=big+sizeof(big)/sizeof(*big) - LDBL_MANT_DIG - 1;

	do {
		*z = y;
		y = 1000000000*(y-*z++);
	} while (y);

	while (e2>0) {
		uint32_t carry=0;
		int sh=MIN(29,e2);
		for (d=z-1; d>=a; d--) {
			uint64_t x = ((uint64_t)*d<<sh)+carry;
			*d = x % 1000000000;
			carry = x / 1000000000;
		}
		if (carry) *--a = carry;
		while (z>a && !z[-1]) z--;
		e2-=sh;
	}
	while (e2<0) {
		uint32_t carry=0, *b;
		int sh=MIN(9,-e2), need=1+(p+LDBL_MANT_DIG/3+8)/9;
		for (d=a; d<z; d++) {
			uint32_t rm = *d & ((1<<sh)-1);
			*d = (*d>>sh) + carry;
			carry = (1000000000>>sh) * rm;
		}
		if (!*a) a++;
		if (carry) *z++ = carry;
		/* Avoid (slow!) computation past requested precision */
		b = (t|32)==U'f' ? r : a;
		if (z-b > need) z = b+need;
		e2+=sh;
	}

	if (a<z) for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
	else e=0;

	/* Perform rounding: j is precision after the radix (possibly neg) */
	j = p - ((t|32)!=U'f')*e - ((t|32)==U'g' && p);
	if (j < 9*(z-r-1)) {
		uint32_t x;
		/* We avoid C's broken division of negative numbers */
		d = r + 1 + ((j+9*LDBL_MAX_EXP)/9 - LDBL_MAX_EXP);
		j += 9*LDBL_MAX_EXP;
		j %= 9;
		for (i=10, j++; j<9; i*=10, j++);
		x = *d % i;
		/* Are there any significant digits past j? */
		if (x || d+1!=z) {
			long double round = 2/LDBL_EPSILON;
			long double small;
			if (*d/i & 1) round += 2;
			if (x<i/2) small=0x0.8p0;
			else if (x==i/2 && d+1==z) small=0x1.0p0;
			else small=0x1.8p0;
			if (pl && *prefix==U'-') round*=-1, small*=-1;
			*d -= x;
			/* Decide whether to round by probing round+small */
			if (round+small != round) {
				*d = *d + i;
				while (*d > 999999999) {
					*d--=0;
					if (d<a) *--a=0;
					(*d)++;
				}
				for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
			}
		}
		if (z>d+1) z=d+1;
	}
	for (; z>a && !z[-1]; z--);
	
	if ((t|32)==U'g') {
		if (!p) p++;
		if (p>e && e>=-4) {
			t--;
			p-=e+1;
		} else {
			t-=2;
			p--;
		}
		if (!(fl&ALT_FORM)) {
			/* Count trailing zeros in last place */
			if (z>a && z[-1]) for (i=10, j=0; z[-1]%i==0; i*=10, j++);
			else j=9;
			if ((t|32)==U'f')
				p = MIN(p,MAX(0,9*(z-r-1)-j));
			else
				p = MIN(p,MAX(0,9*(z-r-1)+e-j));
		}
	}
	l = 1 + p + (p || (fl&ALT_FORM));
	if ((t|32)==U'f') {
		if (e>0) l+=e;
	} else {
		estr=fmt_u(e<0 ? -e : e, ebuf);
		while(ebuf-estr<2) *--estr=U'0';
		*--estr = (e<0 ? U'-' : U'+');
		*--estr = t;
		l += ebuf-estr;
	}

	pad(f, U' ', w, pl+l, fl);
	out(f, prefix, pl);
	pad(f, U'0', w, pl+l, fl^ZERO_PAD);

	if ((t|32)==U'f') {
		if (a>r) a=r;
		for (d=a; d<=r; d++) {
			char32_t *s = fmt_u(*d, buf+9);
			if (d!=a) while (s>buf) *--s=U'0';
			else if (s==buf+9) *--s=U'0';
			out(f, s, buf+9-s);
		}
		if (p || (fl&ALT_FORM)) out(f, U".", 1);
		for (; d<z && p>0; d++, p-=9) {
			char32_t *s = fmt_u(*d, buf+9);
			while (s>buf) *--s=U'0';
			out(f, s, MIN(9,p));
		}
		pad(f, U'0', p+9, 9, 0);
	} else {
		if (z<=a) z=a+1;
		for (d=a; d<z && p>=0; d++) {
			char32_t *s = fmt_u(*d, buf+9);
			if (s==buf+9) *--s=U'0';
			if (d!=a) while (s>buf) *--s=U'0';
			else {
				out(f, s++, 1);
				if (p>0||(fl&ALT_FORM)) out(f, U".", 1);
			}
			out(f, s, MIN(buf+9-s, p));
			p -= buf+9-s;
		}
		pad(f, U'0', p+18, 18, 0);
		out(f, estr, ebuf-estr);
	}

	pad(f, U' ', w, pl+l, fl^LEFT_ADJ);

	return MAX(w, pl+l);
}

static int getint(char32_t **s) {
	int i;
	for (i=0; isdigit(**s); (*s)++)
		i = 10*i + (**s-U'0');
	return i;
}

static int printf_core(struct OutputStream *f, const char32_t *fmt, va_list *ap, union arg *nl_arg, int *nl_type)
{
	char32_t *a, *z, *s=(char32_t *)fmt;
	unsigned l10n=0, fl;
	int w, p;
	union arg arg;
	int argpos;
	unsigned st, ps;
	int cnt=0, l=0;
	int i;
	char32_t buf[sizeof(uintmax_t)*3+3+LDBL_MANT_DIG/4];
	const char32_t *prefix;
	int t, pl;

	for (;;) {
		/* Update output count, end loop when fmt is exhausted */
		if (cnt >= 0) {
			if (l > INT_MAX - cnt) {
				errno = EOVERFLOW;
				cnt = -1;
			} else cnt += l;
		}
		if (!*s) break;

		/* Handle literal text and %% format specifiers */
		for (a=s; *s && *s!=U'%'; s++);
		for (z=s; s[0]==U'%' && s[1]==U'%'; z++, s+=2);
		l = z-a;
		if (f) out(f, a, l);
		if (l) continue;

		if (isdigit(s[1]) && s[2]==U'$') {
			l10n=1;
			argpos = s[1]-U'0';
			s+=3;
		} else {
			argpos = -1;
			s++;
		}

		/* Read modifier flags */
		for (fl=0; (unsigned)*s-U' '<32 && (FLAGMASK&(1U<<(*s-U' '))); s++)
			fl |= 1U<<(*s-U' ');

		/* Read field width */
		if (*s==U'*') {
			if (isdigit(s[1]) && s[2]==U'$') {
				l10n=1;
				nl_type[s[1]-U'0'] = INT;
				w = nl_arg[s[1]-U'0'].i;
				s+=3;
			} else if (!l10n) {
				w = f ? va_arg(*ap, int) : 0;
				s++;
			} else return -1;
			if (w<0) fl|=LEFT_ADJ, w=-w;
		} else if ((w=getint(&s))<0) return -1;

		/* Read precision */
		if (*s==U'.' && s[1]==U'*') {
			if (isdigit(s[2]) && s[3]==U'$') {
				nl_type[s[2]-U'0'] = INT;
				p = nl_arg[s[2]-U'0'].i;
				s+=4;
			} else if (!l10n) {
				p = f ? va_arg(*ap, int) : 0;
				s+=2;
			} else return -1;
		} else if (*s=='.') {
			s++;
			p = getint(&s);
		} else p = -1;

		/* Format specifier state machine */
		st=0;
		do {
			if (OOB(*s)) return -1;
			ps=st;
			st=states[st]S(*s++);
		} while (st-1<STOP);
		if (!st) return -1;

		/* Check validity of argument type (nl/normal) */
		if (st==NOARG) {
			if (argpos>=0) return -1;
		} else {
			if (argpos>=0) nl_type[argpos]=st, arg=nl_arg[argpos];
			else if (f) pop_arg(&arg, st, ap);
			else return 0;
		}

		if (!f) continue;

		z = buf + sizeof(buf);
		prefix = U"-+   0X0x";
		pl = 0;
		t = s[-1];

		/* Transform ls,lc -> S,C */
		if (ps && (t&15)==3) t&=~32;

		/* - and 0 flags are mutually exclusive */
		if (fl & LEFT_ADJ) fl &= ~ZERO_PAD;

		switch(t) {
		case U'n':
			switch(ps) {
			case BARE: *(int *)arg.p = cnt; break;
			case LPRE: *(long *)arg.p = cnt; break;
			case LLPRE: *(long long *)arg.p = cnt; break;
			case HPRE: *(unsigned short *)arg.p = cnt; break;
			case HHPRE: *(unsigned char *)arg.p = cnt; break;
			case ZTPRE: *(size_t *)arg.p = cnt; break;
			case JPRE: *(uintmax_t *)arg.p = cnt; break;
			}
			continue;
		case U'p':
			p = MAX((size_t) p, 2*sizeof(void*));
			t = U'x';
			fl |= ALT_FORM;
		case U'x': case U'X':
			a = fmt_x(arg.i, z, t&32);
			if (arg.i && (fl & ALT_FORM)) prefix+=(t>>4), pl=2;
			if (0) {
		case U'o':
			a = fmt_o(arg.i, z);
			if ((fl&ALT_FORM) && p<z-a+1) p=z-a+1;
			} if (0) {
		case U'd': case U'i':
			pl=1;
			if (arg.i>INTMAX_MAX) {
				arg.i=-arg.i;
			} else if (fl & MARK_POS) {
				prefix++;
			} else if (fl & PAD_POS) {
				prefix+=2;
			} else pl=0;
		case U'u':
			a = fmt_u(arg.i, z);
			}
			if (p>=0) fl &= ~ZERO_PAD;
			if (!arg.i && !p) {
				a=z;
				break;
			}
			p = MAX(p, z-a + !arg.i);
			break;
		case U'c':
			*(a=z-(p=1))=arg.i;
			fl &= ~ZERO_PAD;
			break;
		case U's':
			a = arg.p ? arg.p : U"(null)";
			z = memchr32(a, '\0', p);
			if (!z) z=a+p;
			else p=z-a;
			fl &= ~ZERO_PAD;
			break;
		case U'e': case U'f': case U'g': case U'a':
		case U'E': case U'F': case U'G': case U'A':
			l = fmt_fp(f, arg.f, w, p, fl, t);
			continue;
		}

		if (p < z-a) p = z-a;
		if (w < pl+p) w = pl+p;

		pad(f, U' ', w, pl+p, fl);
		out(f, prefix, pl);
		pad(f, U'0', w, pl+p, fl^ZERO_PAD);
		pad(f, U'0', p, z-a, 0);
		out(f, a, z-a);
		pad(f, U' ', w, pl+p, fl^LEFT_ADJ);

		l = w;
	}

	if (f) return cnt;
	if (!l10n) return 0;

	for (i=1; i<=NL_ARGMAX && nl_type[i]; i++)
		pop_arg(nl_arg+i, nl_type[i], ap);
	for (; i<=NL_ARGMAX && !nl_type[i]; i++);
	if (i<=NL_ARGMAX) return -1;
	return 1;
}

int fprintf32(struct OutputStream * f, const char32_t * fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vfprintf32(f, fmt, ap);
	va_end(ap);
	return ret;
}

int vfprintf32(struct OutputStream * f, const char32_t * fmt, va_list ap)
{
	va_list ap2;
	int nl_type[NL_ARGMAX+1] = {0};
	union arg nl_arg[NL_ARGMAX+1];
	int ret;

	/* the copy allows passing va_list* even if va_list is an array */
	va_copy(ap2, ap);

	if (printf_core(0, fmt, &ap2, nl_arg, nl_type) < 0) {
		va_end(ap2);
		return -1;
	}

	ret = printf_core(f, fmt, &ap2, nl_arg, nl_type);

	va_end(ap2);
	return ret;
}
