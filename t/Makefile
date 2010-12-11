PERL_PATH ?= /usr/bin/perl
TEST_OPTS =

T = $(wildcard *.t)

all: $(T)

$(T):
	@echo "*** $@ ***"; '$(PERL_PATH)' $@ $(TEST_OPTS)

.PHONY: $(T)
