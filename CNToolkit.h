/*
 * brief: �����ࡣ
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014��3��10��
 */
#ifndef CNTOOLKIT_H_H
#define CNTOOLKIT_H_H

#include "kfifo.h"
#include "CNDef.h"

/* 
 * if find a whole packet, return packet ptr; 
 * if not foud, return NULL
 */
struct packet {
	struct list_head list;
	unsigned char *data;
	int len;
};

// brief ��kfifo�ж�ȡһ��������Э������
// param[in] fifoҪ������fifo
// param[in] startЭ�鿪ʼ�ı�־
// param[in] slenЭ�鿪ʼ��־����
// param[in] endЭ�������־
// param[in] elenЭ�������־����
// param[in] buff_len Ҫ�������ڴ泤��
// return ����������Э�������
struct packet *get_packet_from_kfifo( struct kfifo *fifo /* in */,
	const char *start/* in */, int slen /* in */,
	const char *end  /* in */, int elen /* in */, int buff_len /* in */);

// brief ��kfifo�еõ���start��ͷend��β��ָ���
// param[in/OUT] fifo Ҫ�������Ķ��� fifo�ᱻ�޸�
// param[in] start��ʼ��־
// param[in] end������־
struct packet* GetPacket(struct kfifo* IN OUT fifo, const char*  IN start, const char* IN end);

// brief 
// brief �ָ��ַ���
// param[in][out] stringp.Ҫ���ָ���ַ���
// param[in] delim��־�ַ�
// remark �μ�linux��strsep
static inline char* cnstrsep(char** stringp, char delim){
	char*s,*tok;
	if(((s=*stringp)==NULL))
		return NULL;
	for (tok=s;;){
		if (*s++==delim ) {
			s[-1]=0;
			*stringp = s;
			return tok;
		}
		if(*s==0){*stringp=NULL; return tok;}
	}
}

// brief �õ�Э���еĲ�������
// param[in] Frameһ������Э��
// param[in] ����˳��
// param[out] �õ��Ĳ����������
// param[out] ��������
static inline GetParam(char* IN Frame,unsigned int IN nIndex,char* OUT Dest, unsigned int OUT &nLen)
{
	nLen = 0;

	char* p = Frame ;
	for(int i = 0 ; i < nIndex ;i++ )
	{
		p = strstr(p+1,":") ;
		if(p == NULL)
			return false ;
	}

	char* Endp = strstr(p+1, ":") ;
	if(Endp == NULL)
		Endp = strstr(p+1,"\r\n") ;

	if(Endp == NULL)
		return false ;

	nLen = Endp - p-1 ;
	strncpy(Dest, p+1, nLen) ;

	Dest[nLen] = '\0' ;
	return true ;
}

// ���ֽ�BCD��
typedef struct stsingleBCD{
	unsigned char lower:4;
	unsigned char high:4;
	int toInt(bool isSign = false){
		if(!isSign)
			return high * 10 + lower;  
		else
			if( high & 0x8 )
				return -((high - 8) * 10 + lower);
			else
				return high * 10 + lower;
	}
	void toString(char* str, bool isSign = false) {
		if(!isSign) {
			str[0] = high + '0';
			str[1] = lower + '0';
			str[2] = 0;
		} else {
			if( high & 0x8 ) {
				str[0] = '-';
				str[1] = (high-8) + '0';
				str[2] = lower + '0';
				str[3] = 0;
			}
			else {
				str[0] = high + '0';
				str[1] = lower + '0';
				str[2] = 0;
			}
		}
	}
}CNSingleBCD;



#endif