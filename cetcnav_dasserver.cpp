#include "cetcnav_dasserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include "zhelpers.h"
#include "CNToolkit.h"

USECETCNAV

char* g_protocols[]={ 
	"inproc://worker0",
	"inproc://worker1",
	"inproc://worker2",
	"inproc://worker3"
};

int CNDasserver::Init(void* ctx) { 
	if((m_buffer = (unsigned char*)malloc(MAX_FIFO*FIFO_SIZE)) == NULL){
		printf("can't malloc %d bytes %s %d", MAX_FIFO*FIFO_SIZE, __FILE__, __LINE__);
		return -1;
	}
	int i;
	for(i = 0; i < MAX_FIFO; i++){ 
		m_fifo[i]=kfifo_init(FIFO_SIZE, m_buffer+i*FIFO_SIZE);
		assert(m_fifo[i]!=0);
	}
	m_zmqctx = ((struct cetcnav_ctx*)ctx)->zmq_ctx;
	m_socketserver=((struct cetcnav_ctx*)ctx)->socket_server;

	int retval = pthread_create(&m_thread[0], NULL,ResolveData0, (void*)this);
	if (retval !=0) {
		printf("create thread fail %s %d", __FILE__, __LINE__);
		return -1;
	}
	retval = pthread_create(&m_thread[1], NULL,ResolveData1, (void*)this);
	if (retval !=0) {
		printf("create thread fail %s %d", __FILE__, __LINE__);
		return -1;
	}
	retval = pthread_create(&m_thread[2], NULL, ResolveData2, (void*)this);
	if (retval !=0) {
		printf("create thread fail %s %d", __FILE__, __LINE__);
		return -1;
	}
	retval = pthread_create(&m_thread[3], NULL, ResolveData3, (void*)this);
	if (retval !=0) {
		printf("create thread fail %s %d", __FILE__, __LINE__);
		return -1;
	}
	sleep(1);

	return 0;
}

int CNDasserver::Push( struct socket_message& msg ) {
	kfifo_put(m_fifo[msg.id % MAX_FIFO], (unsigned char*)msg.data, msg.ud);	
}

static void ResolveData(int i, void* pobject){
	CNDasserver* pServer=(CNDasserver*)pobject;
	void* zmq_ctx=pServer->m_zmqctx;

	void* sock = zmq_socket(zmq_ctx, ZMQ_PAIR);
	zmq_bind(sock, g_protocols[i]);
	int id = 0;
	struct packet* pPacket = NULL;
	struct list_head *pCurPos, *pTemp, *pList;
	pCurPos=pTemp=pList=NULL;
	CNIncType IncType = NULL;
	CETCNAV::CNReportControl repControl;
	CETCNAV::CNReportLocation repLocation;
	for (;;) {
		char* buffer = s_recv(sock); 
		id = atoi(buffer); 
		pList=GetPacket(pServer->m_fifo[id%MAX_FIFO], "$", "\r\n"); 
		if (pList!=NULL) { 
			list_for_each_safe(pCurPos, pTemp, pList){ 
				pPacket = container_of(pCurPos, struct packet, list); 
				IncType = GetIncType(pPacket->data, pPacket->len);
				switch(IncType){
					case ::CNLOGIN:
					case ::CNHEART:					
					case ::CNREPTERPARAM:
					case ::CNREPMACK:
						DecodeRepControl(IncType, pPacket->data, pPacket->len,repControl);
						repControl->set_id(id);
						break;
					case ::CNREPPOS:
					case ::CNREPROUTE:
					case ::CNREPWARN:
					case ::CNREPBASE:
						DecodeRepLocation(IncType, pPacket->data, pPacket->len, repLocation);
						repLocation->set_id(id);
						break;
				}    
			}        
		}            

		printf("show data from %d %s\n", i, buffer);

		free(buffer);
	}
}

void* CETCNAV::CNDasserver::ResolveData0( void* pobject) { 
	ResolveData(0, pobject));
	return NULL;
}

void* CETCNAV::CNDasserver::ResolveData1( void* pobject) {
	ResolveData(1, pobject);
	return NULL;
}

void* CETCNAV::CNDasserver::ResolveData2( void* pobject) {
	ResolveData(2,pobject);
	return NULL;
}

void* CETCNAV::CNDasserver::ResolveData3( void* pobject) {
	ResolveData(3,pobject);
	return NULL;
}
