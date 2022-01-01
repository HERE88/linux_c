SRC := main.cpp Log.cpp 
INC := Log.h
EXC := testW


CC := arm-none-linux-gnueabihf-g++

CPP_FLAGS += -I/home/wanghaitao/tmp/cx7100_generic/log4cplus-2.0.6/__install/include
CPP_FLAGS += -L/home/wanghaitao/tmp/cx7100_generic/log4cplus-2.0.6/__install/lib


$(EXC):$(SRC) $(INC)
	$(CC) $(SRC) -lpthread -lrt -llog4cplus -std=c++17 $(CPP_FLAGS) -o $@  

clean:
	rm -rf $(EXC)
 