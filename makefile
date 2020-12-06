CC=g++
STD=c++17
CFLAGS=-Wall -Wextra -std=$(STD)
CLIBS=-l chilkat-9.5.0
TARGET_NAME=ir_tp1
TARGET_PATH=$(BUILD_PATH)/$(TARGET_NAME)
BUILD_PATH=./build

all: $(TARGET_NAME)

$(TARGET_NAME): ./main.cpp
	$(CC) $(CFLAGS) -o $(TARGET_PATH) ./main.cpp $(CLIBS)

clean:
	rm -rf $(BUILD_PATH)/*

run:
	$(TARGET_PATH) https://ufmg.br/ 10

mem:
	valgrind --leak-check=full --show-leak-kinds=all $(TARGET_PATH) https://ufmg.br/ 10 > ./output/mem.log