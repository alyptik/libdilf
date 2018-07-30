/*
 * dilf.c - Dynamic Images I'd Like to Format
 *
 * Dynamically load non-shared ELF executables with dlopen().
 *
 * AUTHOR: Joey Pabalinas <joeypabalinas@gmail.com>
 * See LICENSE.md file for copyright and license details.
 */

#include <dilf.h>

/*
 * stdlib abort() wrappers
 */
static inline void xfclose(FILE **restrict out_file)
{
	if (!out_file || !*out_file)
		return;
	if (fclose(*out_file) == EOF)
		WARN("%s", "xfclose()");
}
static inline void xfopen(FILE **restrict file, char const *restrict path, char const *restrict fmode)
{
	if (!(*file = fopen(path, fmode)))
		ERR("%s", "xfopen()");
}
static inline size_t xfread(void *restrict ptr, size_t sz, size_t nmemb, FILE *restrict stream)
{
	size_t cnt;
	if ((cnt = fread(ptr, sz, nmemb, stream)) < nmemb)
		return 0;
	return cnt;
}

/*
 * recursive free()
 */
static inline ptrdiff_t free_vec(char ***restrict vec)
{
	ptrdiff_t cnt;
	if (!vec || !*vec)
		return -1;
	for (cnt = 0; (*vec)[cnt]; cnt++)
		free((*vec)[cnt]);
	free(*vec);
	*vec = NULL;
	return cnt;
}

/*
 * copy src to dest starting at off
 *
 * if `off < 0` then the string it appended
 * after the terminating null-byte.
 */
static inline void strmv(ptrdiff_t off, char *restrict dest, char const *restrict src) {
	ptrdiff_t src_sz;
	char *dest_ptr = dest;
	char const *src_end = src;
	/* sanity checks */
	if (!dest || !src)
		ERRX("%s", "NULL pointer passed to strmv()");
	/* find the end of the source string */
	for (size_t i = 0; i < STR_LIM && *src_end; i++, src_end++);
	/* find the end of the desitnation string if offset is negative */
	if (off < 0)
		for (size_t i = 0; i < STR_LIM && *dest_ptr; i++, dest_ptr++) {}
	else
		dest_ptr = dest + off;
	/* sanity check one more time */
	if (!src_end || !dest_ptr)
		ERRX("%s", "strmv() string not null-terminated");
	src_sz = src_end - src;
	if (src_sz < 0)
		ERRX("%s", "strmv() src_end - src < 0");
	/* copy the data */
	memcpy(dest_ptr, src, (size_t)src_sz + 1);
}
