

all: v2x_ctrl.c
	gcc -c v2x_ctrl.c -lpthread
	ar -r libv2xctl.a v2x_ctrl.o
	gcc main.c libv2xctl.a -lpthread -L. -o test


clean:
	rm v2x_ctrl.o libv2xctl.a test


