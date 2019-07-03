#include<stdio.h>
#include<unistd.h>
#define MAXLINE 80 /* 80 char per command input */

int main(void)
{
	char *args[MAXLINE/2+1];
	int shouldrun = 1;
	while(shouldrun){
		printf("osh>");
		fflush(stdout);
	}
	return 0;
}