SRC := $(wildcard *.cpp) $(wildcard *.h)
CPP := $(wildcard *.cpp)

$(warning source files: $(SRC))

test:$(SRC)
	g++ $(CPP) -lpthread -lrt -std=c++11 -o $@  

clean:
	rm -rf $(EXC)
 