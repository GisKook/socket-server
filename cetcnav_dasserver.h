// ʵ�����ݻ��棬 ��֡�� �ַ�����
#ifndef CETCNAV_DASSERVER_H_H
#define CETCNAV_DASSERVER_H_H
#include "socket_server.h"
#include "CNDef.h"
#include "kfifo.h"
#include <pthread.h>

NSBEGIN

// �����socket_server�ж��������socket��������ͬ��
#define MAX_FIFO_P 16
#define MAX_FIFO (1<<MAX_FIFO_P)  
// ÿ�����л�������ݴ�С ������2��n�η�
#define FIFO_SIZE 8*1024 
// �̸߳���
#define THREAD_COUNT 4
#define WORKER_COUNT THREAD_COUNT

struct cetcnav_ctx{
	void* zmq_ctx;
	void* socket_server_ctx;
	int verbose;
};

class CNDasserver{
public:

	int Init(void* );
	int Push(struct socket_message& msg); 

private:
	static void* ResolveData0(void* );
	static void* ResolveData1(void* );
	static void* ResolveData2(void* );
	static void* ResolveData3(void* );

public:
	struct kfifo* m_fifo[MAX_FIFO];
	unsigned char* m_buffer;
	pthread_t m_thread[THREAD_COUNT];
	void* m_zmqctx;
	void* m_socketserver;
	int m_verbose;
};

NSEND
#endif
