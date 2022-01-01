SRC := $(wildcard *.c)
DIR := $(notdir $(SRC))

INC := $(wildcard *.h)
DIR2 := $(notdir $(INC))

CFLAGS := -lpthread
CC := arm-none-linux-gnueabihf-gcc

.phony: all clean

TARGET = ping
all:$(TARGET)

$(TARGET):$(SRC) $(INC)
	$(CC)  $(SRC) $(CFLAGS) -o $@



clean:
	rm -f $(TARGET)