CC=g++
STD=c++17
CFLAGS=-Wall -Wextra -std=$(STD) -pthread
CLIBS=-l chilkat-9.5.0
TARGET_NAME=web-crawler
TARGET_PATH=$(BUILD_PATH)/$(TARGET_NAME)
BUILD_PATH=./build
SOURCE_PATH=./src
LIB_PATH=./include

SOURCE_EXT := cpp
SOURCES := $(shell find $(SOURCE_PATH) -path '*.$(SOURCE_EXT)')
OBJECTS := $(patsubst $(SOURCE_PATH)/%,$(BUILD_PATH)/%,$(SOURCES:.$(SOURCE_EXT)=.o))

all: $(TARGET_NAME)

$(BUILD_PATH)/%.o: $(SOURCE_PATH)/%.cpp $(LIB_PATH)/%.hpp
	mkdir -p $(BUILD_PATH)
	$(CC) -c $(CFLAGS) -o $@ $< $(CLIBS)

$(TARGET_NAME): $(OBJECTS) ./main.cpp
	$(CC) $(CFLAGS) -o $(TARGET_PATH) ./main.cpp $(OBJECTS) $(CLIBS)

clean:
	rm -rf $(BUILD_PATH)/*

run:
	$(TARGET_PATH) ./input/seed > ./output/crawler.log

reset:
	rm -rf ./output/*.log
	rm -rf ./output/html/*.html

mem:
	valgrind --leak-check=full --show-leak-kinds=all --log-file="./output/mem.log" --max-threads=100 $(TARGET_PATH) ./input/seed > ./output/crawler.log