#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main() {
	int modifyMe = 0;
	char buf[32];
	char *input = NULL;

	input = getenv("MALICIOUS");

	if(input == NULL) {
		printf("Try Again! \nSet the MALICIOUS environment variable\n");
		exit(1);
	}

	strcpy(buf, input);


	if(modifyMe == 0x61626364) {
		printf("modifyMe successfully modified! (´▽`)ﾉ \n");
	}
	else {
		 printf("Try Again (´･⌒･`) \nValue at modifyMe is 0x%08x\n", modifyMe);		
	}
}
