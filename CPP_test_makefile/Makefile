CC := g++

VPATH = src inc
CFLAGS := -I inc -g

SRC := $(wildcard src/*.cpp)
DIR := $(notdir $(SRC))
OBJ_DIR := obj
OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(DIR))

$(shell if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi;)

TARGET=create_bin

.PHONY:all clean

$(OBJ_DIR)/%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

all:$(TARGET)

echo:
	@echo $(SRC)
	@echo $(DIR)
	@echo $(OBJ)

clean:
	rm -rf obj
	rm -f $(TARGET)
