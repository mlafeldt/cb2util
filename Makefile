# The default make target
all:

# Define V=1 to have a more verbose compile.
#
# Define BUILD_MINGW=1 to cross-compile cb2util for Win32 using MinGW.

# Generate version info
CB2UTIL-VERSION-FILE: FORCE
	@./CB2UTIL-VERSION-GEN
-include CB2UTIL-VERSION-FILE
export CB2UTIL_VERSION

prefix = $(HOME)
BIGINT = libbig_int
LIBCHEATS = libcheats

CC = gcc
INSTALL = install
RM = rm -f
SHA1SUM = sha1sum
TAR = tar
ZIP = zip

CFLAGS = -Wall -Werror -O2 -s
CFLAGS += -I$(BIGINT)/include -I$(LIBCHEATS)/include
CFLAGS += -DHAVE_STDINT_H
LDFLAGS =
LIBS =

ifeq ($(BUILD_MINGW),1)
  CC = i586-mingw32msvc-gcc
  CFLAGS += -I$(ZLIB_PATH)/include -L$(ZLIB_PATH)/lib
  LIBS += -lzdll
  PROG = cb2util.exe
else
  LIBS += -lz
  PROG = cb2util
endif

OBJS += $(BIGINT)/src/basic_funcs.o
OBJS += $(BIGINT)/src/bitset_funcs.o
OBJS += $(BIGINT)/src/low_level_funcs/add.o
OBJS += $(BIGINT)/src/low_level_funcs/and.o
OBJS += $(BIGINT)/src/low_level_funcs/andnot.o
OBJS += $(BIGINT)/src/low_level_funcs/cmp.o
OBJS += $(BIGINT)/src/low_level_funcs/div.o
OBJS += $(BIGINT)/src/low_level_funcs/mul.o
OBJS += $(BIGINT)/src/low_level_funcs/or.o
OBJS += $(BIGINT)/src/low_level_funcs/sqr.o
OBJS += $(BIGINT)/src/low_level_funcs/sub.o
OBJS += $(BIGINT)/src/low_level_funcs/xor.o
OBJS += $(BIGINT)/src/memory_manager.o
OBJS += $(BIGINT)/src/modular_arithmetic.o
OBJS += $(BIGINT)/src/number_theory.o
OBJS += $(BIGINT)/src/service_funcs.o
OBJS += $(BIGINT)/src/str_funcs.o

OBJS += $(LIBCHEATS)/src/cheatlist.o
OBJS += $(LIBCHEATS)/src/libcheats.o
OBJS += $(LIBCHEATS)/src/mystring.o
OBJS += $(LIBCHEATS)/src/parser.o

OBJS += arcfour.o
OBJS += cb2_crypto.o
OBJS += cb2util.o
OBJS += cbc.o
OBJS += cheats.o
OBJS += compress.o
OBJS += fileio.o
OBJS += pcb.o
OBJS += shs.o

#
# Quiet build
#
QUIET_SUBDIR0  = +$(MAKE) -C # space to separate -C and subdir
QUIET_SUBDIR1  =

ifneq ($(findstring $(MAKEFLAGS),w),w)
PRINT_DIR = --no-print-directory
else # "make -w"
NO_SUBDIR = :
endif

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
	QUIET_CC       = @echo '   ' CC $@;
	QUIET_AR       = @echo '   ' AR $@;
	QUIET_LINK     = @echo '   ' LINK $@;
	QUIET_BUILT_IN = @echo '   ' BUILTIN $@;
	QUIET_GEN      = @echo '   ' GEN $@;
	QUIET_LNCP     = @echo '   ' LN/CP $@;
	QUIET_GCOV     = @echo '   ' GCOV $@;
	QUIET_SUBDIR0  = +@subdir=
	QUIET_SUBDIR1  = ;$(NO_SUBDIR) echo '   ' SUBDIR $$subdir; \
			 $(MAKE) $(PRINT_DIR) -C $$subdir
	export V
	export QUIET_GEN
	export QUIET_BUILT_IN
endif
endif

$(OBJS): %.o: %.c
	$(QUIET_CC)$(CC) -o $*.o -c $(CFLAGS) $<

all: $(PROG)

install: all
	$(INSTALL) -d -m 755 '$(prefix)/bin/'
	$(INSTALL) $(PROG) '$(prefix)/bin/'

$(PROG): $(OBJS)
	$(QUIET_LINK)$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $(filter %.o,$^) $(LIBS)

cb2util.o: CB2UTIL-VERSION-FILE
cb2util.o: CFLAGS += -DCB2UTIL_VERSION='"$(CB2UTIL_VERSION)"'

clean:
	$(RM) $(PROG) $(OBJS)
	$(RM) CB2UTIL-VERSION-FILE
	$(RM) -r release/
	$(QUIET_SUBDIR0)test $(QUIET_SUBDIR1) clean

test: all
	$(QUIET_SUBDIR0)test $(QUIET_SUBDIR1) all

prove: all
	$(QUIET_SUBDIR0)test $(QUIET_SUBDIR1) prove

PACKAGE = cb2util-$(CB2UTIL_VERSION)
release: all
	$(RM) -r release/
	$(INSTALL) -d -m 755 release/$(PACKAGE)
	$(INSTALL) $(PROG) release/$(PACKAGE)/
ifeq ($(BUILD_MINGW),1)
	$(INSTALL) -m 644 $(ZLIB_PATH)/zlib1.dll release/$(PACKAGE)/
endif
	$(INSTALL) -m 644 README.md CHANGES COPYING release/$(PACKAGE)/
	cd release && \
		$(TAR) -cjf $(PACKAGE).tar.bz2 $(PACKAGE)/; \
		$(ZIP) -qr $(PACKAGE).zip $(PACKAGE)/; \
		$(SHA1SUM) $(PACKAGE).* > $(PACKAGE).sha1

.PHONY: all install clean test prove release
.PHONY: FORCE
