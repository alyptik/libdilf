/*
 * dilf.h - Dynamic Images I'd Like to Format
 *
 * Dynamically load non-shared ELF executables with dlopen().
 *
 * AUTHOR: Joey Pabalinas <joeypabalinas@gmail.com>
 * See LICENSE.md file for copyright and license details.
 */

#if !defined(_LIB_DILF_H)
#define _LIB_DILF_H 1

/* silence linter */
#undef _GNU_SOURCE
#define _GNU_SOURCE

#if defined(__INTEL_COMPILER)
	typedef float _Float32;
	typedef float _Float32x;
	typedef double _Float64;
	typedef double _Float64x;
	typedef long double _Float128;
	typedef enum ___LIB_VERSIONIMF_TYPE {
		_IEEE_ = -1	/* IEEE-like behavior */
		,_SVID_		/* SysV, Rel. 4 behavior */
		,_XOPEN_	/* Unix98 */
		,_POSIX_	/* POSIX */
		,_ISOC_		/* ISO C9X */
	} _LIB_VERSIONIMF_TYPE;
# define _LIB_VERSION_TYPE _LIB_VERSIONIMF_TYPE;
#else
# include <complex.h>
#endif /* defined(__INTEL_COMPILER) */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

/*
 * library version macros
 *
 * for hex-encoded: 0xAABB -> CC.DD (base-10)
 */
#define DILF_VERSION 0x0001
#define DILF_VERSION_MAJOR 0
#define DILF_VERSION_MINOR 1

/*
 * errno, file, and line number macros
 * (do _not_ use parentheses around these)
 */
#define FSTR __FILE__, __LINE__
#define ESTR strerror(errno), FSTR

#define WARN(fmt, args...)	do { fprintf(stderr, "`%s`: [%s:%u] " fmt "\n", ESTR, args); } while (0)
#define WARNX(fmt, args... )	do { fprintf(stderr, "[%s:%u] " fmt "\n", FSTR, args); } while (0)
#define ERR(fmt, args...)	do { fprintf(stderr, "`%s`: [%s:%u] " fmt "\n", ESTR, args); exit(1); } while (0)
#define ERRX(fmt, args...)	do { fprintf(stderr, "[%s:%u] " fmt "\n", FSTR, args); exit(1); } while (0)

/*
 * utility macros
 */
#define STR_LIM 4096
#define MIN(a, b)		(((a) < (b)) ? (a) : (b))
#define MAX(a, b)		(((a) > (b)) ? (a) : (b))
#define ARR_LEN(arr)		((sizeof (arr)) / (sizeof *(arr)))
#define PRINTE(fmt, args...)	fprintf(stderr, "\033[92m" fmt "\033[00m", args)

/*
 * allocator wrappers
 */
#define _xmalloc(type, ptr, sz, msg)			\
	do {						\
		void *tmp = malloc(sz);			\
		if (!tmp)				\
			ERR("%s", (msg));		\
		*(type **)ptr = tmp;			\
	} while (0)
#undef xmalloc
#define xmalloc _xmalloc

#define _xcalloc(type, ptr, nmemb, sz, msg)		\
	do {						\
		void *tmp = calloc((nmemb), (sz));	\
		if (!tmp)				\
			ERR("%s", (msg));		\
		*(type **)ptr = tmp;			\
	} while (0)
#undef xcalloc
#define xcalloc _xcalloc

#define _xrealloc(type, ptr, sz, msg)			\
	do {						\
		void *tmp[2] = {0, *(type **)ptr};	\
		if (!(tmp[0] = realloc(tmp[1], sz)))	\
			ERR("%s", (msg));		\
		*(type **)ptr = tmp[0];			\
	} while (0)
#undef xrealloc
#define xrealloc _xrealloc

#endif /* !defined(_LIB_DILF_H) */
