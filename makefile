CC = g++
SRC_DIR = src
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
H_FILES = $(wildcard $(SRC_DIR)/*.h)
BLISS_DIR = ./include/bliss-0.72/

all: bliss aadage

bliss:
	cd $(BLISS_DIR) && make && cd ../..

aadage: $(CPP_FILES) $(H_FILES)
	$(CC) -I$(BLISS_DIR) -L$(BLISS_DIR) $(CPP_FILES) $(H_FILES) -o aadage -lbliss -lstdc++ -lm -O3
