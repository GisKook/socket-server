// 实现数据缓存， 搜帧， 分发功能
#ifndef CETCNAV_DASSERVER_H_H
#define CETCNAV_DASSERVER_H_H
#include "socket_server.h"
#include "CNDef.h"
#include "kfifo.h"
#include <pthread.h>

NSBEGIN

// 这里和socket_server中定义的最大的socket数必须相同，
#define MAX_FIFO_P 16
#define MAX_FIFO (1<<MAX_FIFO_P)  
// 每个队列缓存的数据大小 必须是2的n次方
#define FIFO_SIZE 8*1024 
// 线程个数
#define THREAD_COUNT 4

class CNDasserver{
public:

	int Init();
	int Push(struct socket_message& msg); 
	int ForwordMessage();

private:
//	static void* ResolveData0(void* );
//	static void* ResolveData1(void* );
//	static void* ResolveData2(void* );
//	static void* ResolveData3(void* );

private:
	struct kfifo* m_fifo[MAX_FIFO];
	unsigned char* m_buffer;
	//pthread_t m_thread[THREAD_COUNT];
};

NSEND
#endif
