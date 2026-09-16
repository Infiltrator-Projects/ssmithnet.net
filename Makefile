CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

.PHONY: all clean check

all: sitegen
	./build/sitegen .

sitegen: src/sitegen.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o build/sitegen

check: sitegen
	./build/sitegen .
	@grep -q '<title>Shannon Smith' index.html
	@grep -q 'E 250 CDI BlueEFFICIENCY Coupé' garage.html
	@grep -q 'The LINK family.' workbench.html
	@grep -q 'The old internet is part of the story.' archive.html
	@echo "sitegen checks passed"

clean:
	rm -rf build
