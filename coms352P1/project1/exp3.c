#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define BUFFER_SIZE 50

char history[10][BUFFER_SIZE];
int count = 0;  /* Number of commands entered*/
char historyArr[50]; /*Holds the commands fetched from history on request*/


void printHistory()
{
	int i,k;
	int j = 0;
	int countSec = count; /* second count to list out history */
	for (i = 0; i<10;i++)
	{
		 k=0;
  		 printf("%d.  ", countSec);
		 while (history[i][j] != '\n' && history[i][j] != '\0')
		 {
			  printf("%c", history[i][ j]);
			  j++;
		 }
		 printf("\n");
		 int j = 0;
		 countSec--;
		 if (countSec ==  0)
			break;
	}
	printf("\n");


} 

/*
  setup() reads in the next command line, separating it into distinct tokens
using whitespace as delimiters. setup() sets the args parameter as a
 null-terminated string.
 */


void setup(char in_buffer[], char *args[],int *background)
{
	int length; /* # of charaindexers in the command line */
	int i;     /* loop index for accessing in_buffer array */
	int indexStart;  /* index where the start of next command parameter is */
	int index;     /* index of where to place the next parameter into args[] */
	index = 0;
	char histFlag[7];
	/* read what the user enters on the command line */
	length = read(STDIN_FILENO, in_buffer, MAX_LINE);

	/*logic to not insert !! and !N in history array*/
	if(length >=7)
	{
		memset(&histFlag, 0, sizeof(histFlag)); /* Array is reset*/
		strncpy ( histFlag, in_buffer, 7 );
	}

	if(in_buffer[0] != '!' && strcmp("prompt:",histFlag)!=0) 
	{
		for (i = 9;i>0; i--)
			strcpy(history[i], history[i-1]);
		strcpy(history[0],in_buffer);
		count++;
	}
	indexStart = -1;
	if (length == 0)
		exit(0);            

	if (length < 0)
	{
		perror("error reading the command\n");
		exit(-1);           /* if length is less than zero terminate with an error*/
	}

	/* Checking every character in the in_buffer */
	for (i=0;i<length;i++)
	{
		switch (in_buffer[i])
		{
			case ' ':
			case '\t' :               /* tabs is argument separators */
				if(indexStart != -1)
				{
					args[index] = &in_buffer[indexStart];    /* set up pointer */
					index++;
				}
				in_buffer[i] = '\0'; /* adding a null char to make a C string */
				indexStart = -1;
			break;

			case '\n':                 /* endline character is final character */
				if (indexStart != -1 && *background==0)
				{
					args[index] = &in_buffer[indexStart];
					index++;
				}
				in_buffer[i] = '\0';
				args[index] = NULL; /* Indicates end of command */
			break;

			default :             /* any other character */
				if (indexStart == -1)
					indexStart = i;
				if (in_buffer[i] == '&')
				{
					*background  = 1;
					in_buffer[i] = '\0';
				}
		}
	}
	args[index] = NULL; /* if it exceeds size of 80 */
}

int main(void)
{
	system("clear");
	char in_buffer[MAX_LINE]; /* to store the input commands*/
	int background;             /*is set if '&' is given at end of command */
	char *args[MAX_LINE/+1];/* to have 40 arguments */
	char checkCommand[10]; /* To check the input like !! & etc*/
	pid_t pid;
	int i;
	int shouldrun = 1;
	//char *check = args[0];
	//char token;
	while (shouldrun)  //while runs always, unless user enter exit
	{           
		background = 0;
		printf("osh>\n");
		fflush(stdout);
		setup(in_buffer,args,&background);  /* To get next command */
		if(strcmp(args[0],"exit")==0)	//if user enters exit, it exits out
		{
			printf("exiting, will take a moment to exit!\n");
			shouldrun =0;
			exit(0);
		}
		int k,j,statusFlag ;
		k = 0;
		j=0;
		int historyCount =0; /* This is loop counter to traverse through history array*/
		int lengthArgs = strlen(args[0]); 
		statusFlag =0; /* This is to track whether command entered will be executed or not by execvp*/
		int no_commands=0;
		int historyFlag = 0; /* This flag is to test whether !! or !N has been given as input or not*/
		int maxIndex = count;
		int minIndex =0;  
		if(count>9)
		minIndex = count - 9;
		char* histHold[20];
		strcpy(checkCommand,args[0]);
		int position_value = 0; /* This will hold what position will fetched from history */
		if(lengthArgs <= 2)
			position_value = (int)checkCommand[1] - '0'; /* Convert the character digit to numeric value*/

		/**
			*This else loop will work in this way: if !10 is given then position to be fetched from is 10
			* It will extract the value given after '!' and will store in variable position_value
		*/		
		else
		{
			int power = lengthArgs -2;	
			int value = 0;
			int loop = 0;
			int hold;
			/**
				* This for loop takes value N from !N
				* It then parse each digit from N and forms the numeric value
				* If input is given as !10 then it will take out 12
				* It will parse each digit (1 and 0)
				* It will form the numeric value as 1*(10 power of 1) + 2 * (10 power of 0)
			*/	
			for(loop=1;loop<lengthArgs;loop++)
			{
				hold = (int)checkCommand[loop] -'0';
				value = value + hold * pow(10,power);
				power--;
			}
			position_value = value;
		}
		// /* this is to check whether there are multiple commands*/
		// while((token = strtok_r(NULL, ";",&check)))
		// {
		// 	execvp(args[0], token);
		// }
				
		/* This block will fetch the command from history only when the input pattern will be like !! or !N*/
		if(checkCommand[0] == '!' )
		{
			if((checkCommand[1] == '!' && count>0) || 
				(position_value >= minIndex 
					&& position_value <= maxIndex))
			{
				int position = 0; /* This is initialized to 0 to get the latest command from history*/
				/*Check for the pattern !N*/
				if(checkCommand[1]!='!')
				{
					position =  maxIndex - position_value;
				}
				memset(&historyArr[0], 0, sizeof(historyArr)); /* Array initialization*/
				/**
					* This while loop will take each character (except null and enter) from history array
					* It will put the character in a temporary 1D array
				*/
				while (history[position][j] != '\n' && history[position][j] != '\0')
				{
					historyArr[k] = history[position][j];
					j++;
					k++;
				}

				/**
					*This is to make an entry in the history array 
					* with the command corresponding to the position !! 
					* or !N
				*/
				for (historyCount = 9; historyCount>0; historyCount--)
					strcpy(history[historyCount], history[historyCount-1]);

				strcpy(history[0],historyArr);
				count++;
				printf("Command fetched: %s\n",historyArr);

				/** 
					* This block will get the command from the history for the given position.
					* Commands are spliited by space if it contains more than one word
					* It will store the command in a pointer of character array
					* The pointer character array is passed to execvp to execute
				*/
				int histArrLength = strlen(historyArr); /* Get the length of the command fetched from history*/
				int arrCount = 0;
				char holdChar; /* This will hold the each character of the command while parsing the command*/
				int histHoldCounter = 0;
				int blank = 0; /* This is handle multiple blanks between words in a command*/
				histHold[histHoldCounter] = (char*) calloc(1, sizeof(char)); /* Dynamic Initialization*/
				/**
					* This for loop will parse the temporary 1D array
					* characters are concatenated and put in the current index of array of character pointers
					* If the character is space then next index of pointer array is initialized
				*/
				for (arrCount =0 ;arrCount<histArrLength; arrCount++)
				{
					holdChar = historyArr [arrCount];
					if(holdChar != ' ')
					{
						histHold[histHoldCounter] = strcat(histHold[histHoldCounter],&holdChar);
						blank = 1;						
					}
					if(holdChar == ' ')
					{
						//handle multiple blank
						if(blank == 1 )
						{
							histHoldCounter++;
							histHold[histHoldCounter] = (char*) calloc(1, sizeof(char));
							blank = 0;
						}
					}
				}
				/* Set the immediate next index as zero after populating it with the parsed command words*/
				histHold[histHoldCounter + 1 ] = NULL; 
				statusFlag  = 1;
			 	historyFlag = 1;
				/* Block ends here */ 
			}
			else 
			{
				if(count == 0)
					printf("No commands in history\n");
				else 
				{
					printf("No such command in histoy\n");
					no_commands = 1; //flag used to stop creating processes if no commands to be execute
				}
				statusFlag  = 1; //flag used to stop executing execvp if no commands to be execute
			}
		}
		// This block is to print the history of commands 
		if(strcmp("prompt",args[0])==0) 
		{
			if(count>0)
			{
				printHistory();
				statusFlag  = 1;
			}else
			{
				printf("No commands in history\n");
				statusFlag  = 1;
			}

		}
		//conditions to check when to create processes
		if((strcmp("prompt",args[0])!=0) && (no_commands == 0)) 
			pid = fork();
		int pid_child = 0;
		if (pid < 0)
		{
			printf("Fork failed.\n");
			exit (1);
		}

		//the child process will invoke execvp()
		else if (pid == 0)
		{
			//getting the pid of child process
			pid_child = getpid(); 
			if (historyFlag == 1)
			{
				statusFlag  = 1;
				//executing of command fetched from history
				execvp (histHold[0], histHold); 
			}
			if(statusFlag  == 0) 
			{
				//checking whether command has executed properly or not.
				//If it cannot execute, displaying error executing command
				if (execvp (args[0], args) == -1) 
				{
					printf("Error executing command\n"); 
					kill(pid_child,SIGKILL);
					//if command gives an error, 
					//the child process has to be killed explicitly. 
				}
			}
		}

		/**
			* if background == 0, the parent will wait,
			* else continue to returns to the setup() function.
		*/
		else
		{
			if (background == 0)
			waitpid(pid,NULL,0);
		}
	}
}