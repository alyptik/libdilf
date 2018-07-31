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
 * stdlib error-checked wrappers
 */
static void xfclose(FILE **restrict out_file)
{
	if (!out_file || !*out_file) {
		return;
	}
	if (fclose(*out_file) == EOF) {
		print_err(&errno, "%s", "xfclose()");
	}
}

static void xfopen(FILE **restrict file, char const *restrict path, char const *restrict fmode)
{
	if (!(*file = fopen(path, fmode))) {
		print_err(&errno, "%s", "xfopen()");
	}
}

static size_t xfread(void *restrict ptr, size_t sz, size_t nmemb, FILE *restrict stream)
{
	size_t cnt = fread(ptr, sz, nmemb, stream);
	return cnt < nmemb ? 0 : cnt;
}

/*
 * recursive free()
 */
static ptrdiff_t free_vec(char ***restrict vec)
{
	ptrdiff_t cnt;
	char **vec_ptr = *vec;
	if (!vec || !*vec) {
		return -1;
	}
	for (cnt = 0; vec_ptr[cnt]; cnt++) {
		free(vec_ptr[cnt]);
	}
	free(vec_ptr);
	*vec = 0;
	return cnt;
}

/*
 * copy src to dest starting at off
 *
 * if `off < 0` then the string it appended
 * after the terminating null-byte.
 */
static void strmv(ptrdiff_t off, char *restrict dest, char const *restrict src) {
	ptrdiff_t src_sz;
	char *dest_ptr = dest;
	char const *src_end = src;
	if (!dest || !src) {
		print_err(0, "%s", "NULL passed to strmv()");
		return;
	}
	/* find the end of the source string */
	for (size_t i = 0; i < STR_LIM && *src_end; i++, src_end++) {}
	/* find the end of the destination string if offset is negative */
	if (off < 0) {
		for (size_t i = 0; i < STR_LIM && *dest_ptr; i++, dest_ptr++) {}
	} else {
		dest_ptr = dest + off;
	}
	/* do sanity checks before copy */
	if (!src_end || !dest_ptr) {
		print_err(0, "%s", "strmv() string not null-terminated");
		return;
	}
	src_sz = src_end - src;
	if (src_sz < 0) {
		print_err(0, "%s", "strmv() src_end - src < 0");
		return;
	}
	memcpy(dest_ptr, src, (size_t)src_sz + 1);
}

/*
 * `struct str_list` helper functions
 */
static ssize_t free_str_list(struct str_list *restrict plist)
{
	size_t null_cnt = 0;
	/* return -1 if passed NULL */
	if (!plist || !plist->list) {
		return -1;
	}
	for (size_t i = 0; i < plist->cnt; i++) {
		/* if NULL increment counter and skip */
		if (!plist->list[i]) {
			null_cnt++;
			continue;
		}
		free(plist->list[i]);
		plist->list[i] = 0;
	}
	free(plist->list);
	plist->list = 0;
	plist->cnt = 0;
	plist->max = 1;
	return null_cnt;
}

static void init_str_list(struct str_list *restrict list_struct, char *restrict init_str)
{
	list_struct->cnt = 0;
	list_struct->max = 1;
	xcalloc(1, list_struct->list, sizeof *list_struct->list, "list_ptr calloc()");
	if (!init_str || !list_struct->list) {
		abort();
	}
	list_struct->cnt++;
	xcalloc(1, list_struct->list[list_struct->cnt - 1], strlen(init_str) + 1, "init_str_list()");
	strmv(0, list_struct->list[list_struct->cnt - 1], init_str);
}

static void append_str(struct str_list *restrict list_struct, char const *restrict string, size_t pad)
{
	if (!list_struct->list) {
		print_err(0, "%s", "0 list_struct->list passed to append_str()");
		return;
	}
	/* realloc the next power of two if cnt reaches current size */
	if (++list_struct->cnt >= list_struct->max) {
		size_t old_max = list_struct->max;
		list_struct->max <<= 1;
		if (list_struct->max < old_max) {
			print_err(0, "%s", "append_str() size wrapped");
			abort();
		}
		xrealloc(list_struct->list, sizeof *list_struct->list * list_struct->max, "append_str()");
	}
	if (!list_struct->list) {
		return;
	}
	if (!string) {
		list_struct->list[list_struct->cnt - 1] = 0;
		return;
	}
	xcalloc(1, list_struct->list[list_struct->cnt - 1], strlen(string) + pad + 1, "append_str()");
	if (!list_struct->list[list_struct->cnt - 1]) {
		return;
	}
	strmv(pad, list_struct->list[list_struct->cnt - 1], string);
}

/*
 * memfd helper functions
 */
static int init_memfd(void)
{
	int mem_fd;
	if ((mem_fd = syscall(SYS_memfd_create, "libdilf_memfd", MFD_CLOEXEC)) < 0) {
		print_err(&errno, "%s", "error creating mem_fd");
	}
	return mem_fd;
}

static void set_cloexec(int set_fd[static 2])
{
	if (fcntl(set_fd[0], F_SETFD, FD_CLOEXEC) < 0) {
		print_err(&errno, "%s", "fnctl()");
	}
	if (fcntl(set_fd[1], F_SETFD, FD_CLOEXEC) < 0) {
		print_err(&errno, "%s", "fnctl()");
	}
}

static void pipe_fd(int in_fd, int out_fd)
{
	for (;;) {
		ssize_t ret;
		if ((ret = splice(in_fd, 0, out_fd, 0, PAGE_SIZE, SPLICE_F_MOVE)) < 0) {
			/* handle interrupted syscalls and non-blocking fds */
			if (errno == EINTR || errno == EAGAIN) {
				continue;
			}
			print_err(&errno, "%s", "error reading from input fd");
			abort();
		}
		/* EOF */
		if (!ret) {
			break;
		}
	}
}
