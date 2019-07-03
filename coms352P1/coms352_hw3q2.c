#include // exit()
#include //fork()
#include // pid_t
#include // wait()
#include // printf()
#include 
#include 
#include 
#include // shm_open(), ftruncate(), mmap(), shm_unlink()
#include 

int main(int argc, char* argv[]){
const int SIZE = 4096;
const char *name = "OS";
static int count = 0;
if(argc !=2 ){
    perror("usage error");
    exit(-1);
}
int num = atoi(argv[1]);

int shm_fd;
void *ptr;
shm_fd = shm_open(name, O_CREAT, 0666);
ftruncate(shm_fd, SIZE);

if( num > 0)
{
    pid_t pid;
    pid = fork();
    if(pid == 0)
    {
        int shm_fd;
        void *ptr;
        shm_fd = shm_open(name, O_RDWR, 0666);
        ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

        sprintf(ptr, "%d", num);
        ptr += sizeof(int);
        count ++;

        while(num != 1)
        {
            num = (num%2 == 0 ? num/2 : 3*num+1);
            sprintf(ptr, "%d", num);
            ptr += sizeof(int);
            count++;
        }
    }
    else if(pid >0)
    {
        wait(NULL);

        int shm_fd;
        void *ptr;
        shm_fd = shm_open(name, O_RDONLY, 0666);
        ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        int *p = (int*) ptr;
        printf("%d ", *p++);
        while(count > 0){
            printf("%d ",*p++);
            count --;
        }

        shm_unlink(name);
        printf("\n");
    }
    else
    {
        perror("fork error");
        exit(-1);
    }   
}
else
{
    perror("please input a positive interge");
    exit(-1);
}   

return 0;
}