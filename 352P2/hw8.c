#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <fcntl.h> 
int main( )					
{				
int fp;
char buff[20];
 int pid;
					
pid = fork( );
if (pid==0)				
{
fp = open("sample", O_RDONLY); 
printf("child begins %d\n" , getpid( )); read(fp,buff,10);
buff[10] = '\0';
printf("child read: ");
puts(buff);
printf("\nchild exits");
}				
else 
{					
fp = open("sample", O_RDONLY);
printf("parent begins %d\n" , getpid( )); 
read(fp,buff,10);
buff[10] = '\0';				
printf("parent read: "); 
puts(buff); 
printf("parent exits\n");					
} 
}
