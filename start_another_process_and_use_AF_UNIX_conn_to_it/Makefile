SRC := $(wildcard *.c)
DIR := $(notdir $(SRC))
EXC := $(patsubst %.c,%,$(SRC))

all:$(EXC)

%:%.c
	gcc -g $< -lpthread -lm -lutil -lrt -o $@  

clean:
	rm -rf $(EXC)
 