# config.mk - Dynamic Images I'd Like to Format
#
# Dynamically load non-shared ELF executables with dlopen().
#
# AUTHOR: Joey Pabalinas <joeypabalinas@gmail.com>
# See LICENSE.md file for copyright and license details.

# add -Wrestrict if using gcc7 or higher
RESTRICT := $(shell gcc -v 2>&1 | awk '/version/ { if (substr($$3, 1, 1) > 6) { print "-Wrestrict"; } }')

# optional
DESTDIR ?=
PREFIX ?= /usr/local
CC ?= gcc
OLVL ?= -O3

# mandatory
LD = $(CC)
DEBUG_CFLAGS = $(CFLAGS) $(DEBUG)
DEBUG_LDFLAGS = $(LDFLAGS) $(DEBUG)
MKALL = $(MKCFG) $(DEP)
OBJ = $(SRC:.c=.o)
TOBJ = $(TSRC:.c=.o)
DEP = $(SRC:.c=.d) $(TSRC:.c=.d)
TEST = $(filter-out $(TAP),$(TSRC:.c=))
UTEST = $(filter-out src/$(TARGET).o,$(SRC:.c=.o))
SRC := $(wildcard src/*.c)
TSRC := $(wildcard t/*.c)
HDR := $(wildcard src/*.h) $(wildcard t/*.h)
ASAN := -fsanitize=address,alignment,leak,undefined
CPPFLAGS := -D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -MMD -MP
TARGET := libdilf
MANPAGE := dilf.3
TAP := t/tap
BINDIR := bin
MANDIR := share/man/man3
WARNINGS := $(RESTRICT) -Wall -Wextra -pedantic				\
		-Wcast-align -Wfloat-equal -Wmissing-declarations	\
		-Wmissing-prototypes -Wnested-externs -Wpointer-arith	\
		-Wshadow -Wstrict-overflow
IGNORES := -Wno-conversion -Wno-cpp -Wno-discarded-qualifiers		\
		-Wno-implicit-fallthrough -Wno-inline -Wno-long-long	\
		-Wno-missing-field-initializers -Wno-redundant-decls	\
		-Wno-sign-conversion -Wno-strict-prototypes		\
		-Wno-unused-variable -Wno-write-strings
LDLIBS += -D_GNU_SOURCE -D_DEFAULT_SOURCE
LDLIBS += -ldl -lelf
MKALL += Makefile asan.mk
DEBUG += -O1 -no-pie -D_DEBUG
DEBUG += -fno-builtin -fno-inline
CFLAGS += -g3 -std=c11 -fPIC
CFLAGS += -fstack-protector-strong
CFLAGS += -fuse-ld=gold -fuse-linker-plugin
CFLAGS += -fno-common -fno-strict-aliasing
CFLAGS += $(WARNINGS) $(IGNORES)
LDFLAGS += -Wl,-O3,-z,relro,-z,now,-z,noexecstack
LDFLAGS += $(filter-out $(WARNINGS),$(CFLAGS))

# vi:ft=make:
