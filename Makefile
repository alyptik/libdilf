# libdilf - Dynamic Images I'd Like to Format
#
# Dynamically load non-shared ELF executables with dlopen().
#
# AUTHOR: Joey Pabalinas <joeypabalinas@gmail.com>
# See LICENSE.md file for copyright and license details.

all:
	$(MAKE) $(TARGET) check

# user configuration
MKCFG := config.mk
export C_INCLUDE_PATH=$(READLINE)/include
# if previously built with `-fsanitize=address` we have to use `ASAN` flags
ASAN_OPT != test -f asan.mk
ifeq ($(.SHELLSTATUS),0)
	OLVL = $(ASAN)
endif
-include $(DEP) $(MKCFG)
.PHONY: all asan check clean debug dist install test uninstall $(MKALL)

asan:
	# asan indicator flag
	@touch asan.mk
	$(MAKE) clean
	$(MAKE) $(TARGET) check OLVL= CFLAGS="$(DEBUG_CFLAGS)" LDFLAGS="$(DEBUG_LDFLAGS)"

debug:
	$(MAKE) $(TARGET) check OLVL= CFLAGS="$(DEBUG_CFLAGS)" LDFLAGS="$(DEBUG_LDFLAGS)"

$(TARGET): %: $(OBJ)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST): %: %.o $(TAP).o $(OBJ) $(TOBJ)
	$(LD) $(LDFLAGS) $(TAP).o $(<:t/test%=src/%) $< $(LDLIBS) -o $@

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) $(OLVL) $(CPPFLAGS) -c $< -o $@

test check: $(TEST)
	@echo "[running unit tests]"
	# ./t/testdilf

clean:
	@echo "[cleaning]"
	$(RM) $(DEP) $(TARGET) $(TEST) $(OBJ) $(TOBJ) $(TARGET).tar.gz cscope.* tags TAGS asan.mk

install: $(TARGET)
	@echo "[installing]"
	mkdir -p $(DESTDIR)$(PREFIX)/$(BINDIR)
	mkdir -p $(DESTDIR)$(PREFIX)/$(MANDIR)
	install -c $(TARGET) $(DESTDIR)$(PREFIX)/$(BINDIR)
	install -c $(MANPAGE) $(DESTDIR)$(PREFIX)/$(MANDIR)

uninstall:
	@echo "[uninstalling]"
	$(RM) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(TARGET)
	$(RM) $(DESTDIR)$(PREFIX)/$(MANDIR)/$(MANPAGE)

dist: clean
	@echo "[creating dist tarball]"
	mkdir -p $(TARGET)/
	cp -R LICENSE.md Makefile README.md $(HDR) $(SRC) $(TSRC) $(MANPAGE) $(TARGET)/
	tar -czf $(TARGET).tar.gz $(TARGET)/
	$(RM) -r $(TARGET)/

cscope:
	@echo "[creating cscope database]"
	$(RM) cscope.*
	cscope -Rub

tags TAGS:
	@echo "[creating ctags file]"
	$(RM) tags TAGS
	ctags -RGf $@ \
		--langmap=c:+.h.C.H --fields=+l \
		--c-kinds=+l-p --c++-kinds=+l-p --python-kinds=+lz \
		--extras=+q --tag-relative=no .

# vi:ft=make:
