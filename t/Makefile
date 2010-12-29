SHELL_PATH ?= $(SHELL)
PROVE ?= prove
RM ?= rm -f
DEFAULT_TEST_TARGET ?= test

TEST_OPTS =
PROVE_OPTS =

T = $(wildcard t[0-9][0-9][0-9][0-9]-*.sh)

all: $(DEFAULT_TEST_TARGET)

test: pre-clean
	$(MAKE) aggregate-results-and-cleanup

prove: pre-clean
	@echo "*** prove ***"; $(PROVE) --exec '$(SHELL_PATH)' $(PROVE_OPTS) $(T) :: $(TEST_OPTS)
	$(MAKE) clean

$(T):
	@echo "*** $@ ***"; '$(SHELL_PATH)' $@ $(TEST_OPTS)

pre-clean:
	$(RM) -r test-results

clean:
	$(RM) -r test-results
	$(RM) .prove

aggregate-results-and-cleanup: $(T)
	$(MAKE) aggregate-results
	$(MAKE) clean

aggregate-results:
	@for f in test-results/t*-*.counts; do \
		echo "$$f"; \
	done | '$(SHELL_PATH)' ./aggregate-results.sh

.PHONY: all test prove $(T) pre-clean clean
.PHONY: aggregate-results-and-cleanup aggregate-results
