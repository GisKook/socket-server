#include "cetcnav_dasserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

USECETCNAV

int CNDasserver::Init() { 
	if((m_buffer = (unsigned char*)malloc(MAX_FIFO*FIFO_SIZE)) == NULL){
		printf("can't malloc %d bytes %s %d", MAX_FIFO*FIFO_SIZE, __FILE__, __LINE__);
		return -1;
	}
	int i;
	for(i = 0; i < MAX_FIFO; i++){ 
		m_fifo[i]=kfifo_init(FIFO_SIZE, m_buffer+i*FIFO_SIZE);
		assert(m_fifo[i]!=0);
	}

//	int retval = pthread_create(m_thread[0], NULL,(void*) ResolveData0, (void*)this);
//	if (retval !=0) {
//		printf("create thread fail %s %d", __FILE__, __LINE__);
//		return -1;
//	}
//	retval = pthread_create(m_thread[1], NULL,(void*) ResolveData1, (void*)this);
//	if (retval !=0) {
//		printf("create thread fail %s %d", __FILE__, __LINE__);
//		return -1;
//	}
//	retval = pthread_create(m_thread[2], NULL,(void*) ResolveData2, (void*)this);
//	if (retval !=0) {
//		printf("create thread fail %s %d", __FILE__, __LINE__);
//		return -1;
//	}
//	retval = pthread_create(m_thread[3], NULL,(void*) ResolveData3, (void*)this);
//	if (retval !=0) {
//		printf("create thread fail %s %d", __FILE__, __LINE__);
//		return -1;
//	}

	return 0;
}

int CNDasserver::Push( struct socket_message& msg ) {
	kfifo_put(m_fifo[msg.id % MAX_FIFO], (unsigned char*)msg.data, msg.ud);	
}
