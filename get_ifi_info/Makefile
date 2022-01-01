
EXC := test001
SRC :=  test001.c  get_ifi_info.c  sock_ntop_host.c unpifi.h unp.h
		
		
all:$(EXC)

$(EXC):$(SRC)
	gcc -g $^ -lpthread -lm -lutil -lrt -o $@  

clean:
	rm -rf $(EXC)
 