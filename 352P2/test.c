#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	char* p1 = "http://google.com/serch.html";
    char* p2 = strstr("http://google.com/serch.html", "/");
    printf("%.*s\n",p2);
while(NULL != p2){
    printf("%.*s\n", p2 - p1, p1);
    p1 = p2;
    p2 = strstr(p2 + 1, "\r\n\r\n");
    
}

}