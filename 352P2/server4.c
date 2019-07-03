/**httpserver.c**/
#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include"time.h"
#include"dirent.h"
#include"netdb.h"

#define BUF_SIZE 2048
#define CLADDR_LEN 100

int create_socket(char * host, int port);
int requestListening(int sockfd);
char * get_Filetype(char * file);

int main(int argc, char **argv) { 
    DIR * dirptr;
    FILE * fileptr;
    time_t current_time8;
    struct tm * timeinfo;
    time (&current_time8);
    timeinfo = localtime(&current_time8);

    char * header, * request, * path, * path2, * host;
    char * dir, * temp;
    int port, sockfd, connfd;
    char get[3], http[9];
    char filepath[BUF_SIZE];
    char http_not_found[] = "HTTP/1.1 404 Not Found\n";
    char http_ok[] = "HTTP/1.1 200 OK\n";
    char buffer[BUF_SIZE];
    char * contentType;

    if (argc != 4) {
        printf("usage: [host] [directory] [portnumber]\n");
        exit(1);
    }

    header = (char*)malloc(BUF_SIZE*sizeof(char));
    request = (char*)malloc(BUF_SIZE*sizeof(char));
    path = (char*)malloc(BUF_SIZE*sizeof(char));
    path2 = (char*)malloc(BUF_SIZE*sizeof(char));

    host = argv[1];
    dir = argv[2];
    port = atoi(argv[3]);

    if ((dirptr = opendir(dir)) == NULL) {
        printf("Directory Not Found!\n");
        exit(1);
    }

    sockfd = create_socket(host, port);

    for (;;) {
        printf("***********************************************\n");
        printf("Waiting for a connection now...\n");
        connfd = requestListening(sockfd);
        //gets the request from the connection 
        recv(connfd, request, 100, 0);
        printf("Processing the request...\n");
        //parses the request
        sscanf(request, "%s %s %s", get, path, http);
        //this is to ignore the first slash
        path2 = path + 1; 
        sprintf(filepath,"%s/%s", dir, path2);
        contentType = get_Filetype(path2);
        sprintf(header, "Date: %sHostname: %s:%d\nLocation: %s\nContent-Type: %s\n\n", asctime(timeinfo), host, port, path2, contentType);
        if ((fileptr = fopen(filepath, "r")) == NULL ) 
        {
            printf("Error, file not found!\n");
            //sends HTTP 404
            send(connfd, http_not_found, strlen(http_not_found), 0); 
        } 
        else {
            printf("Sending the file now...\n");
            //sends HTTP 200 OK  
            send(connfd, http_ok, strlen(http_ok), 0); 
            recv(connfd, buffer, BUF_SIZE, 0);
            if ((temp = strstr(buffer, "OK")) == NULL) 
            {
                printf("Operation aborted!\n");
                break;
            }
            //sends the header
            send(connfd, header, strlen(header), 0); 
            recv(connfd, buffer, BUF_SIZE, 0);
            if ((temp = strstr(buffer, "OK")) == NULL) 
            {
                printf("Operation aborted!\n");
                break;
            }
            memset(&buffer, 0, sizeof(buffer));
            while (!feof(fileptr)) { 
                //sends the file
                fread(&buffer, sizeof(buffer), 1, fileptr);
                send(connfd, buffer, sizeof(buffer), 0);
                memset(&buffer, 0, sizeof(buffer));
            }
            printf("File is sent.\n");
        }
        printf("Processing completed.\n");
        close(connfd);
    }

    close(sockfd);
    free(header);
    free(request);
    free(path);
    free(path2);
    return 0;
}

int create_socket(char * host, int port) {
    int sockfd;
    struct sockaddr_in addr;
    struct hostent * host_ent;
    char * hostAddr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((short)port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {  
        printf("Error creating socket!\n");  
        exit(1);  
    }  
    printf("Socket is created.\n");
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Error binding socket to port!\n");  
        exit(1);
    }
    printf("Binding is done.\n");
    return sockfd;
}

int requestListening(int sockfd) {
    int conn;
    char hostip[32];
    struct sockaddr_in addr;
    struct hostent * host;
    struct in_addr inAddr;
    int len;
    addr.sin_family = AF_INET;
    //maximum of 2 connections at a time
    listen(sockfd, 2); 
    len = sizeof(addr); 
    if ((conn = accept(sockfd, (struct sockaddr *)&addr, &len)) < 0) 
    {
        printf("Error, cannot accept connection!\n");
        exit(1);
    }
    printf("Connection is accepted.\n");
    inet_ntop(AF_INET, &(addr.sin_addr), hostip, 32);
    inet_pton(AF_INET, hostip, &inAddr);
    host = gethostbyaddr(&inAddr, sizeof(inAddr), AF_INET);
    printf("****Connection received from: %s [IP= %s]****\n", host->h_name, hostip);
    return conn;
}

char * get_Filetype(char * file) {
    char * temp;
    if ((temp = strstr(file, ".html")) != NULL) 
    {
        return "text/html";
    } 
}
