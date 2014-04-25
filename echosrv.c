#include "socket_server.h"
#include "cetcnav_dasserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zmq.h>
#include "zhelpers.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

USECETCNAV

int main(void)
{
	struct socket_server* ss = socket_server_create();
	int listen_id = socket_server_listen(ss, 100, "", 8888, 32);
	socket_server_start(ss, 200, listen_id);

	void* zmq_ctx = zmq_ctx_new();
	assert(zmq_ctx!=NULL); 
	struct cetcnav_ctx ctx={zmq_ctx, ss};
	CNDasserver dasserver;
	dasserver.Init((void*)&ctx);

	char* protocols[]={
		"inproc://worker0",
		"inproc://worker1",
		"inproc://worker2",
		"inproc://worker3"
	};
	void* zmq_sockets[WORKER_COUNT];
	int i;
	for (i = 0; i < WORKER_COUNT; i++) {
		zmq_sockets[i]=zmq_socket(zmq_ctx, ZMQ_PAIR);
		zmq_connect(zmq_sockets[i], protocols[i]);
	}
	
	// 事件循环
	struct socket_message result;
	for (;;) {
		int type = socket_server_poll(ss, &result, NULL);
		// DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
		switch (type) {
		case SOCKET_EXIT:
			goto EXIT_LOOP;
		case SOCKET_DATA:
			{
				printf("message(%" PRIuPTR ") [id=%d] size=%d\n",result.opaque,result.id, result.ud);
				dasserver.Push(result);
				// 给缓存发信号
				char buf[10]={0};
				sprintf(buf, "%d", result.id);
				switch (result.id % 4) {
					case 0: 
						s_send(zmq_sockets[0], buf);
						break; 
					case 1:
						s_send(zmq_sockets[1], buf); 
						break;
					case 2:
						s_send(zmq_sockets[2], buf); 
						break;
					case 3:
						s_send(zmq_sockets[3], buf);
						break;
				}

				printf(".");
				free(result.data);
			}
			break;
		case SOCKET_CLOSE:
			//printf("close(%" PRIuPTR ") [id=%d]\n",result.opaque,result.id);
			break;
		case SOCKET_OPEN:
			//printf("open(%" PRIuPTR ") [id=%d] %s\n",result.opaque,result.id,result.data);
			break;
		case SOCKET_ERROR:
			//printf("error(%" PRIuPTR ") [id=%d]\n",result.opaque,result.id);
			break;
		case SOCKET_ACCEPT:
			//printf("accept(%" PRIuPTR ") [id=%d %s] from [%d]\n",result.opaque, result.ud, result.data, result.id);
			socket_server_start(ss, 300, result.ud);
			break;
		}
	}

EXIT_LOOP:
	socket_server_release(ss);
	return 0;
}
