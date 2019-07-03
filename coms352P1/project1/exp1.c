#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 80 //80 char per command input 
#define BufferSize 50

char history[10][BufferSize];//history array to store history commands
int count = 0;

/**
  *this main(void) will read in command line and separate into distinct tokens
  *by using delimiters which is the whitespace. args is the parameter set as a
  *null-terminated string.
  */

int main(void)
{
  char *args[MAXLINE/2 + 1];
  char s[MAXLINE];
  int shouldrun = 1;
  int i;
  int j;

  while(shouldrun)
  {
        printf("osh> ");
        fflush(stdout);
        scanf ("%[^\n]%*c", s);//read in input
        printf("input:%s\n",s);//print the input
        i = 0;
        int j;
        args[i] = strtok(s," ");
        while (args[i] != NULL) {
          i++;
          args[i] = strtok(NULL, " ");
        }

        if(strcmp(args[0], "exit") == 0)
          break;
        if(strcmp(args[i-1], "&") != 0)
        {
          pid_t pid;
          pid = fork();
          if(pid < 0)
          {
                fprintf(stderr,"FORK Failed\n");
                return 1;
          }
          else if (pid == 0)
          {
                execvp(args[0],args);
                for(int j=0;j<i;j++)
                  args[j] = NULL;
          }
          else
          {
                wait(NULL);
          }
        }
        else
        {
          pid_t pid;
          pid = fork();
          if(pid < 0){
                fprintf(stderr,"FORK Failed\n");
                return 1;
          }
          else if (pid == 0){
                args[i-1] = NULL;
                execvp(args[0],args);
          }
          else
          {
                printf("\n\n");
          }
        }

  }
  return 0;
}