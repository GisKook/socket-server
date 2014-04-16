socket-server : echosrv.c socket_server.c cetcnav_dasserver.cpp kfifo.c
	g++ -g -Wall -o $@ $^ -lpthread

clean:
	rm socket-server
