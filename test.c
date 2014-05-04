#include "stdlib.h"
#include "stdio.h"

int main(){
	char str[16]={0};
	memcpy((void*)str, "123456789012345", 15);
	long long i=atoll(str);
	printf("%llu\n", i);
}
