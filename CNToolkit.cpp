#include "CNToolkit.h"
#include "list.h"

struct list_head* GetPacket( struct kfifo* IN OUT fifo, const char* IN start, const char* IN end ) { 
	assert(fifo!=NULL); 
	char* pBuffer = fifo->buffer;
	assert(pBuffer!=NULL); 
	struct list_head* pList=NULL;
	int nLen = kfifo_len(fifo);
	int nLen_start=strlen(start);
	int nLen_end=strlen(end);
	assert(memcmp((void* )pBuffer, (void*)start, nLen_start)==0);
	char *s=NULL, *e=NULL, *cur=NULL;
	int i,j;
	for (i = 0; i<nLen; i++){ 
		cur=pBuffer+i;
		if (*cur==*start) {
			if (memcmp((void*)cur, (void*)start, nLen_start)==0) {
				s=cur;
				i+=nLen_start-1;
				continue;
			}
		}
		if (s!=NULL&&*cur==*end) {
			if (memcmp((void*)cur, (void*)end, nLen_end)==0) {
				e=cur+nLen_end; 
				i+=nLen_end-1;
			}
		}
		if (s!=NULL&&e!=NULL) { 
			assert(s==pBuffer);
			struct packet* pPacket=(struct packet*)malloc(sizeof(*pPacket));
			assert(pPacket!=NULL);
			pPacket->len = e-s;
			pPacket->data = malloc(pPacket->len); 
			assert(pPacket->data!=NULL);
			kfifo_get(fifo, pPacket->data, pPacket->len);
			if (pList==NULL) { 
				pList= (struct list_head *) malloc( sizeof(struct list_head) );
				assert(pList!=NULL);
				INIT_LIST_HEAD(pList);
			}
			list_add_tail(&pPacket->list, pList);
			s=e=NULL;
		}
	}
	
	return pList;
}
