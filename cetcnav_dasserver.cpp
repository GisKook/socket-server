#include "cetcnav_dasserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include "zhelpers.h"
#include "CNToolkit.h"
#include "CNReportControl.pb.h"
#include "CNReportLocation.pb.h"
#include "mdp_common.h"
#include "mdp_client.h"
#include "CNProtocolPersonPos.h"
#include <string>

USECETCNAV

using namespace std;

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
	m_socketserver=((struct cetcnav_ctx*)ctx)->socket_server_ctx;
	m_verbose = ((struct cetcnav_ctx*)ctx)->verbose;

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
	// 从socket_server接收消息
	void* sock = zmq_socket(zmq_ctx, ZMQ_PAIR);
	zmq_bind(sock, g_protocols[i]);
	
	// 使用管家模式进行发送
	mdp_client_t *session=mdp_client_new("tcp://127.0.0.1:5555", 1, zmq_ctx);

	int id = 0;
	struct packet* pPacket = NULL;
	struct list_head *pCurPos, *pTemp, *pList;
	pCurPos=pTemp=pList=NULL;
	::CNIncType IncType = ::CNNULL;
	CETCNAV::CNReportControl repControl;
	CETCNAV::CNReportLocation repLocation;
	zpoller_t *poller=zpoller_new(sock, mdp_client_getsocket(session), NULL);
	assert(poller);
	string str;
	for (;;) {
		void* which = zpoller_wait(poller, -1);
		if (which == sock) {
			char* buffer = s_recv(sock); 
			id = atoi(buffer); 
			pList=GetPacket(pServer->m_fifo[id%MAX_FIFO], "$", "\r\n"); 
			zmsg_t *request = zmsg_new ();
			if (pList!=NULL) { 
				list_for_each_safe(pCurPos, pTemp, pList){ 
					pPacket = container_of(pCurPos, struct packet, list); 
					IncType = GetIncType(reinterpret_cast<const char*>(pPacket->data), pPacket->len);
					switch(IncType){
					case ::CNLOGIN:
					case ::CNHEART:					
					case ::CNREPTERPARAM:
					case ::CNREPMACK:
						DecodeRepControl(IncType, pPacket->data, pPacket->len,repControl);
						repControl.set_id(id);
						repControl.SerializeToString(&str); 
				        zmsg_pushstr (request,str.c_str()); 
				        mdp_client_send (session, "dps", &request); 
						break;
					case ::CNREPPOS:
					case ::CNREPROUTE:
					case ::CNREPWARN:
					case ::CNREPBASE:
						DecodeRepLocation(IncType, pPacket->data, pPacket->len, repLocation);
						repLocation.set_id(id);
						repLocation.SerializeToString(&str); 
				        zmsg_pushstr (request, str.c_str());
				        mdp_client_send (session, "dps", &request); 
						break;
					default:
						assert(0);
						break;
					}    
				}        
			}
			free(buffer);
			free(pList);
		    buffer=NULL;
			pList=NULL;

		}else if(which == mdp_client_getsocket(session)){ 
			char* command, *service;
			zmsg_t *reply = mdp_client_recv(session, &command, &service);
			if (reply) {
				printf ("从管家收到消息\n");
				zmsg_destroy(&reply);
			}
			free(command);
			free(service);
			command=service=NULL;
		}
	}

	zpoller_destroy(&poller);
	mdp_client_destroy(&session);
	session = NULL;
}

void* CETCNAV::CNDasserver::ResolveData0( void* pobject) { 
	// 使用管家模式进行连接
	ResolveData(0, pobject);
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
