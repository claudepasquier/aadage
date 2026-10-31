CC = g++
CXXFLAGS ?= -O3 -Wall -Wextra
DEBUG_FLAGS = -g -O0 -DDEBUG -Wall -Wextra
SRC_DIR = src
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
H_FILES = $(wildcard $(SRC_DIR)/*.h)
BLISS_DIR = ./include/bliss-0.72/

all: bliss aadage

bliss:
	cd $(BLISS_DIR) && $(MAKE) && cd ../..

aadage: $(CPP_FILES) $(H_FILES)
	$(CC) $(CXXFLAGS) -I$(BLISS_DIR) -L$(BLISS_DIR) $(CPP_FILES) $(H_FILES) -o aadage -lbliss -lstdc++ -lm

# unoptimized build with debug symbols
debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: aadage

# run the unit tests (see tests/README.md)
test: aadage
	cd tests && $(MAKE) test

clean:
	rm -f aadage
	cd $(BLISS_DIR) && $(MAKE) clean && cd ../..
	cd tests && $(MAKE) clean && cd ..

.PHONY: all bliss aadage debug test clean
