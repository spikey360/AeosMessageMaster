all: aeoscligui aeosclient

aeoscligui: aeoscligui.c aeoscli.o conlist.o
	gcc `pkg-config --cflags --libs gtk+-3.0` -oaeoscligui aeoscligui.c aeoscli.o conlist.o -lbluetooth -lpthread

aeoscli.o: aeoscli.c
	gcc -c aeoscli.c -lbluetooth

conlist.o: conlist.c
	gcc -c conlist.c

aeosclient: aeosclient.c aeoscli.o
	gcc -oaeosclient aeosclient.c aeoscli.o -lbluetooth

