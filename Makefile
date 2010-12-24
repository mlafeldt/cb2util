all:

# Generate version info
CB2UTIL-VERSION-FILE: FORCE
	@./CB2UTIL-VERSION-GEN
-include CB2UTIL-VERSION-FILE
export CB2UTIL_VERSION

BIGINT = libbig_int
LIBCHEATS = libcheats

CC = gcc
CFLAGS = -Wall -Werror -O2 -s
CFLAGS += -I$(BIGINT)/include -I$(LIBCHEATS)/include
CFLAGS += -DHAVE_STDINT_H
LIBS =
prefix = $(HOME)

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

all: $(PROG)

install: all
	install $(PROG) $(prefix)/bin

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

cb2util.o: CB2UTIL-VERSION-FILE
cb2util.o: CFLAGS += -DCB2UTIL_VERSION='"$(CB2UTIL_VERSION)"'

clean:
	rm -f $(PROG) $(OBJS)
	rm -rf release/
	rm -f CB2UTIL-VERSION-FILE

test: all
	$(MAKE) -C t/ all

PACKAGE = cb2util-$(CB2UTIL_VERSION)
release: all
	rm -rf release
	mkdir -p release/$(PACKAGE)
	cp $(PROG) release/$(PACKAGE)/
ifeq ($(BUILD_MINGW),1)
	cp $(ZLIB_PATH)/zlib1.dll release/$(PACKAGE)/
endif
	cp CHANGES COPYING README release/$(PACKAGE)/
	cd release && \
		tar -cjf $(PACKAGE).tar.bz2 $(PACKAGE)/; \
		zip -qr $(PACKAGE).zip $(PACKAGE)/; \
		sha1sum $(PACKAGE).* > $(PACKAGE).sha1

.PHONY: FORCE
