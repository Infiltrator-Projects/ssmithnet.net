CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

.PHONY: all clean check

all: sitegen
	./build/sitegen .

sitegen: src/sitegen.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o build/sitegen

check: sitegen
	python3 tests/check_site.py

clean:
	rm -rf build

