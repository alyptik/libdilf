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
#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <limits.h>
#include <linux/memfd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * NULL-terminated string dynamic array
 */
struct str_list {
	size_t cnt, max;
	char **list;
};

/*
 * library version macros
 *
 * for hex-encoded: 0xAABB -> CC.DD (base-10)
 */
#define DILF_VERSION 0x0001
#define DILF_VERSION_MAJOR 0
#define DILF_VERSION_MINOR 1

/*
 * utility macros
 */
#undef typeof
#define typeof __typeof__
#define STR_LIM 4096
#define PAGE_SIZE 4096


/*
 * print_dbg - print a debug message with color escapes
 */
#define print_dbg(fmt, args...) \
	fprintf(stderr, "\033[92m"fmt"\033[00m", args)

/*
 * print_err - print an error message if err_ptr is a pointer to errno
 */
#define print_err(err_ptr, fmt, args...)					\
	do {									\
		if ((err_ptr) == &errno) {					\
			fprintf(stderr, "%s: ", strerror(errno));		\
		}								\
		fprintf(stderr, "[%s:%u] "fmt"\n", __FILE__, __LINE__, args);	\
	} while (0)

/*
 * comparison macros
 */
#define type_check(x, y) \
	(!!(sizeof((typeof(x) *)1 == (typeof(y) *)1)))
#define is_constexpr(x) \
	(sizeof(int) == sizeof(*(8 ? ((void *)((long)(x) * 0l)) : (int *)8)))
#define is_arr(arr) \
	((arr) && !__builtin_types_compatible_p(typeof(arr), typeof(&(arr)[0])))
#define cmp(x, y, op) \
	((x) op (y) ? (x) : (y))

/*
 * min_t - return minimum of two values, using the specified type
 */

#define min_t(type, x, y)		cmp((type)(x), (type)(y), <)
#define min(x, y)			cmp(x, y, <)

/*
 * max_t - return maximum of two values, using the specified type
 */
#define max_t(type, x, y)		cmp((type)(x), (type)(y), >)
#define max(x, y)			cmp(x, y, >)

/*
 * clamp_t - return a value clamped to a given range using a given type
 */
#define clamp_t(type, val, lo, hi)	min_t(type, max_t(type, val, lo), hi)
#define clamp(val, lo, hi)		min((typeof(val)) max(val, lo), hi)

/*
 * arr_len - array length or 0 if not an array
 */
#define arr_len(arr)								\
	__builtin_choose_expr(is_arr(arr),					\
		0,								\
		(sizeof (arr)) / (sizeof *(arr)))				\

/*
 * error-checked allocator wrappers
 */
#define _xmalloc(ptr, sz, msg)							\
	do {									\
		(ptr) = malloc((sz));						\
		if (!(ptr)) {							\
			print_err(&errno, "%s", msg);				\
		}								\
	} while (0)
#undef xmalloc
#define xmalloc _xmalloc

#define _xcalloc(nmemb, ptr, sz, msg)						\
	do {									\
		(ptr) = calloc((nmemb), (sz));					\
		if (!(ptr)) {							\
			print_err(&errno, "%s", msg);				\
		}								\
	} while (0)
#undef xcalloc
#define xcalloc _xcalloc

#define _xrealloc(ptr, sz, msg)							\
	do {									\
		typeof(ptr) _tmp;						\
		if (!(_tmp = realloc((ptr), (sz)))) {				\
			print_err(&errno, "%s", msg);				\
			free(ptr);						\
		}								\
		(ptr) = _tmp;							\
	} while (0)
#undef xrealloc
#define xrealloc _xrealloc

#endif /* !defined(_LIB_DILF_H) */
