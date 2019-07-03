#include <string.h>   /* strlen                         */
#include <unistd.h>   /* fork, pipe, read, write, close */
#include <sys/wait.h> /* wait                           */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
int pid;
char buffer[1024];
int fd[2],fd1[2];

pipe(fd); /* fd[0] is for read, fd[1] is for write */
pipe(fd1);
pid = fork();

if (pid == 0) /* process1 */
{
    int count;
    close(fd[0]); /* close unused end, process1 will write */

      /* prompt user for input */
    printf("input: ");
    fgets(buffer, sizeof(buffer), stdin);
    printf("process1: original message is %s", buffer);

      /* write to the pipe (include NUL terminator) */
    count = write(fd[1], buffer, strlen(buffer) + 1);

    count = read(fd1[0], buffer, sizeof(buffer));
    printf("process1: inverted message recieved from process2 is %s", buffer);

     exit(0);
}
else /* process2 */
{
    int count,i;
    close(fd[1]); /* close unused end, process2 will read */

      /* read from the pipe */
    count = read(fd[0], buffer, sizeof(buffer));
    printf("process2: message recieved from process1 is %s", buffer);

    for(i = 0;i<sizeof(buffer);i++){
        if(islower(buffer[i]))
                buffer[i] = toupper(buffer[i]);
        else
                buffer[i] = tolower(buffer[i]);
        }
    printf("process2: inverted message sent to process1 is %s", buffer);
    count = write(fd1[1], buffer, strlen(buffer) + 1);

    wait(NULL);   /* reap the process1 */
}

return 0;
}