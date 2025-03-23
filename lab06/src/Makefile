CXX = g++
CXXFLAGS = -Wall -Wextra -pthread

TARGETS = q01

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
	@rm -f $(TARGETS)

%:
	@:


.PHONY: all clean run