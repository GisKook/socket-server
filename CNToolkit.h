/*
 * brief: 工具类。
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014年3月10日
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

// brief 从kfifo中得到以start开头end结尾的指令表
// param[in/OUT] fifo 要被分析的队列 fifo会被修改
// param[in] start开始标志
// param[in] end结束标志
struct list_head* GetPacket(struct kfifo* IN OUT fifo, const char*  IN start, const char* IN end);

// brief 
// brief 分割字符串
// param[in][out] stringp.要被分割的字符串
// param[in] delim标志字符
// remark 参见linux的strsep
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

// brief 得到协议中的参数数据
// param[in] Frame一个完整协议
// param[in] 参数顺序
// param[out] 得到的参数结果缓存
// param[out] 参数长度
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

// 单字节BCD码
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