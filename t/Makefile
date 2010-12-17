SHELL_PATH ?= /bin/sh
TEST_OPTS =

T = $(wildcard t[0-9][0-9][0-9][0-9]-*.sh)

all: $(T)

$(T):
	@echo "*** $@ ***"; '$(SHELL_PATH)' $@ $(TEST_OPTS)

.PHONY: $(T)
