BIGINT = ../../libbig_int

CC = gcc
CFLAGS = -Wall -Werror -O2 -s
CFLAGS += -I$(BIGINT)/include
LIBS =
prefix = $(HOME)

PROG = CB2cheats
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
OBJS += CB2cheats.o
OBJS += arcfour.o
OBJS += cb2_crypto.o

all: $(PROG)

install: $(PROG)
	install $(PROG) $(prefix)/bin

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

clean:
	$(RM) $(PROG) $(OBJS)
