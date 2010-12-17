SHELL_PATH ?= $(SHELL)
PROVE ?= prove

TEST_OPTS =
PROVE_OPTS =

T = $(wildcard t[0-9][0-9][0-9][0-9]-*.sh)

all: $(T)

prove:
	@echo "*** prove ***"; $(PROVE) --exec '$(SHELL_PATH)' $(PROVE_OPTS) $(T) :: $(TEST_OPTS)

$(T):
	@echo "*** $@ ***"; '$(SHELL_PATH)' $@ $(TEST_OPTS)

.PHONY: $(T)
