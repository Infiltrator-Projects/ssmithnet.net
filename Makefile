CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
COMMON_DIR ?= ../Libraries
COMMON_BUILD_DIR := $(abspath build/common)
COMMON_ARCHIVE := $(COMMON_BUILD_DIR)/libinfiltratr-common.a
COMMON_WEB_CSS := assets/infiltrator-web-v1.css

.PHONY: all clean check sitegen common common-web

all: sitegen
	./build/sitegen .

common:
	$(MAKE) -C "$(COMMON_DIR)" BUILD_DIR="$(COMMON_BUILD_DIR)" all

common-web:
	cp "$(COMMON_DIR)/design/infiltrator-web-v1.css" "$(COMMON_WEB_CSS)"

sitegen: common common-web src/sitegen.cpp include/site-family.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -I"$(COMMON_DIR)/include" src/sitegen.cpp "$(COMMON_ARCHIVE)" -lm -ldl -o build/sitegen

build/check-web-family: tests/check_web_family.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/check_web_family.cpp -o build/check-web-family

check: sitegen build/check-web-family
	cmp "$(COMMON_WEB_CSS)" "$(COMMON_DIR)/design/infiltrator-web-v1.css"
	python3 tests/check_site.py
	./build/check-web-family .

clean:
	rm -rf build
