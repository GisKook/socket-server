#include "CNToolkit.h"
#include "list.h"

unsigned char *packet_buff = (unsigned char *)malloc(KFIFO_BUFF_SIZE * 1024 + 1);
/* fetch a valid packet list from kfifo--loop buffer, last reset loop buffer offset tag */
struct packet *get_packet_from_kfifo(struct kfifo *fifo /* in */, 
	const char *start/* in */, int slen /* in */, 
	const char *end  /* in */, int elen /* in */,
	int buff_len /* in */)
{ 
	if (unlikely(fifo == NULL  || kfifo_len(fifo) <= MIN_PACKET_LEN))
		return NULL;

	char *out_begin = NULL,*out_end = NULL, *p, *in_begin = NULL, *in_end =NULL ;
	struct list_head *packet_list_ptr = NULL ;

	int in_counter   = 0;
	int out_counter  = 0;
	unsigned int len = 0;

	int sflag = 0;
	int eflag = 0;

	memset(packet_buff, 0,  KFIFO_BUFF_SIZE * 1024 + 1 );
	len = kfifo_data(fifo, packet_buff, buff_len);		
	if ( len == 0 ){
		return NULL;
	}

	assert( len <= KFIFO_BUFF_SIZE * 1024 );

	unsigned int i = 0;
	in_begin = p = (char *)packet_buff; 

	while( i < len ){
		sflag = 0;
		eflag = 0;
		for( int sloop = 0; sloop < slen; ++sloop){
			if ( *(p + sloop) == *(start + sloop) )
			{
				sflag++;
			}
		}

		if ( sflag == slen ){
			out_begin = p;				
		} else if ( out_begin != NULL ){
			for( int eloop = 0; eloop < elen; ++eloop){
				if ( *(p + eloop) == *(end + eloop) )
				{
					eflag++;
				}
			}
			if( eflag == elen){
				out_end = p ;	
				int pack_len = out_end - out_begin + elen;	

				struct packet *item = (struct packet *)malloc( sizeof( struct packet ) );
				if (item == NULL )		
					return (struct packet *)packet_list_ptr;
				item->data = (unsigned char *)malloc( pack_len + 20 );
				memset(item->data, 0,         pack_len + 20 );
				memcpy(item->data, out_begin, pack_len  ); 
				item->len = pack_len ;

				if (packet_list_ptr == NULL){
					packet_list_ptr = (struct list_head *) malloc( sizeof(struct list_head) );
					if ( packet_list_ptr == NULL )
						return NULL;

					INIT_LIST_HEAD( packet_list_ptr );				
				}

				list_add_tail( &item->list,  packet_list_ptr );
				out_begin = NULL;
				out_end = NULL;
				out_counter++;	
				in_begin = p + elen;
			}
		}

		p++;
		i++;	
	};

	int del_len = in_begin -(char *) packet_buff;
	if ( del_len > 0  && (unsigned int)del_len <= len ){
		kfifo_add_out(fifo, del_len);
	}
	DEBUG_INFO(DEBUG_2, "get packet total %d in packets , %d out packets", in_counter, out_counter);

	return (struct packet*)packet_list_ptr;
};

struct packet* GetPacket( struct kfifo* IN OUT fifo, const char* IN start, const char* IN end )
{ 
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
	
	return (struct packet*)pList;
}
