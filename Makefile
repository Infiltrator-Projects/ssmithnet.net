CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
COMMON_DIR ?= ../Libraries
COMMON_BUILD_DIR := $(abspath build/common)
COMMON_ARCHIVE := $(COMMON_BUILD_DIR)/libinfiltratr-common.a
COMMON_WEB_CSS := assets/infiltrator-web-v1.css

.PHONY: all clean check sitegen common common-web gallery-photos

all: sitegen gallery-photos
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

build/check-site: tests/check_site.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/check_site.cpp -o build/check-site

build/prepare-garage-photos: tools/prepare_garage_photos.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Werror $(shell pkg-config --cflags libjpeg libwebp) $< -o $@ $(shell pkg-config --libs libjpeg libwebp)

gallery-photos: build/prepare-garage-photos
	./build/prepare-garage-photos assets/photos

check: sitegen gallery-photos build/check-web-family build/check-site
	@! find src tools tests -type f -name '*.py' | grep -q . || { echo 'Python source is forbidden in website-owned code' >&2; exit 1; }
	cmp "$(COMMON_WEB_CSS)" "$(COMMON_DIR)/design/infiltrator-web-v1.css"
	./build/check-site .
	./build/check-web-family .

clean:
	rm -rf build
