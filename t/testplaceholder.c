/*
 * t/test<template>.c - unit-test for <template>.c
 *
 * AUTHOR: Joey Pabalinas <alyptik@protonmail.com>
 * See LICENSE.md file for copyright and license details.
 *
 * __OR__
 *
 * Copyright 2017 Joey Pabalinas <alyptik@protonmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#define _SVID_SOURCE
#define _XOPEN_SOURCE 700

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include "tap.h"
#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <regex.h>
#include <signal.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <unistd.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#define _Atomic
#define _Static_assert(a, b)
#define UNUSED __attribute__ ((unused))

extern char **environ;

int main(int argc, char **argv)
{
	(void)argc, (void)argv;

	plan(1);

	ok(1 == 1, "<template>");

	/* return handled */
	done_testing();
}
