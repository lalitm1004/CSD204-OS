CXX = g++
CXXFLAGS = -Wall -Wextra -pthread

TARGETS = q01_1 q01_2 q02 q03

all: $(TARGETS)

%: %.cpp
	@$(CXX) $(CXXFLAGS) $< -o $@

run:
	@if [ "$(filter-out $@,$(MAKECMDGOALS))" = "" ]; then \
		echo "Please specify a target to run (e.g., 'make run q01_1')"; \
	else \
		$(MAKE) -s $(filter-out $@,$(MAKECMDGOALS)) >/dev/null && ./$(filter-out $@,$(MAKECMDGOALS)); \
	fi

clean:
	@rm -f $(TARGETS) *.csv

help:
	@echo "Available commands:"
	@echo "  make           - Build all programs"
	@echo "  make q01_1     - Build q01_1"
	@echo "  make run q01_1 - Build and run q01_1"
	@echo "  make clean     - Remove built programs"

%:
	@:

.PHONY: all clean help run
