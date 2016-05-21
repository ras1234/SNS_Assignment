#include<stdio.h>
#include<string.h>

int main() {
	int modifyMe = 0;
	char buf[32];

	printf("Provide input \n");
	gets(buf);	

	if(modifyMe == 0x41434547) 
	{
		printf("modifyMe successfully modified! (´▽`)ﾉ \n");
	}
	else {
		printf("Try Again (´･⌒･`) \nValue at modifyMe is 0x%08x\n", modifyMe);
	}

	return 0;
}


