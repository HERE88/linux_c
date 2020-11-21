SRC := $(wildcard *.c)
DIR := $(notdir $(SRC))
OBJ := 	$(patsubst %.c, %.o, $(DIR))

CFLAGS := -lpthread

.phony: all clean

TARGET = test123
all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ)