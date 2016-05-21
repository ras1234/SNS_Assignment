#include<stdio.h>
#include<string.h>

void tryToExecute() {
	printf("tryToExecute sucessfully executed (/^▽^)/ \n");
}

int main() {
	int (*functionPointer)() = NULL;
	char buf[32];

	gets(buf);

	if(functionPointer) {
		printf("Jumping to address 0x%08x\n", functionPointer);
		functionPointer();
	}
	else {
		printf("Try Again! ⊙︿⊙ \nfunctionPointer doesn't point to any address\n");
	}
}
