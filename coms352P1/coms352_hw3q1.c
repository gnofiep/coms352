#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	printf("\nCollatz Conjecture \n");

	int num;
	int process;

	printf("Enter a number greater than 0 to run the Collatz Conjecture.\n"); 
  	scanf("%d", &num);
  		if (num <0){
  			printf("This number is a negative number!.\n");
  		}	
  		else{
  			process = fork();

  			if(process <0){
  				printf("Child process is not created.\n");
  				return 1;
  			}
  			else if( process ==0){
  				printf("Child process is running.\n");
  				printf( "\n%d \n", num);
  				do{
  					if(num %2 != 0){
  						num = (num*3)+1;
  						printf( "%d \n", num);
  					}
  					else{
  						num = num/2;
  						printf( "%d \n", num);
  					}
  				}while(num != 1);
  			}
  			else{
  				printf("Parent is waiting on child process.\n");
  				wait(NULL);
  				printf("Parent process is done.\n");
  			}
  		}
  		return 1;
}
