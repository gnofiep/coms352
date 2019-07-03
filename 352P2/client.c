#include"stdio.h"    
#include"stdlib.h"    
#include"sys/types.h"    
#include"sys/socket.h"    
#include"string.h"    
#include"netinet/in.h"    
#include"netdb.h"  
    
#define BUF_SIZE 2048   
 
int isValid_IP(char * ip);    
int get_request(char * url, char * port);   
int parse_header(char * header);  
void openFile();  
char * split_key_val(char * line, int index);  

FILE * fileptr;  
char keys[][25] = {"Date: ", "Hostname: ", "Location: ", "Content-Type: "};  
char status[4] = {0, 0, 0, 0};  
char contentFileType[100];  
char path[1000];    
  
int isValid_IP(char * ip) 
{  
 struct sockaddr_in addr;  
 int valid = inet_pton(AF_INET, ip, &(addr.sin_addr));  
 return valid != 0;  
}  

int get_request(char * url, char * port) 
{  
  
 int sockfd, bindfd;  
 char * ptr, * host;  
 char getrequest[1024];  
 struct sockaddr_in addr;  
  
 if (isValid_IP(url)) { //when an IP address is given  
  sprintf(getrequest, "GET / HTTP/1.1\nHOST: %s\n\n", url);  
        } else { //when a host name is given  
  if ((ptr = strstr(url, "/")) == NULL) {  
   //when hostname does not contain a slash  
   sprintf(getrequest, "GET / HTTP/1.1\nHOST: %s\n\n", url);  
  } else {  
   //when hostname contains a slash, it is a path to file  
   strcpy(path, ptr);  
          host = strtok(url, "/");  
   sprintf(getrequest, "GET %s HTTP/1.1\nHOST: %s\n\n", path, url);  
  }  
 }   
  
 // creates a socket to the host  
 sockfd = socket(AF_INET, SOCK_STREAM, 0);  
 if (sockfd < 0) {    
  printf("Error creating socket!\n");    
  exit(1);    
 }    
 printf("Socket is created.\n");  
 memset(&addr, 0, sizeof(addr));    
 addr.sin_family = AF_INET;    
 addr.sin_addr.s_addr = inet_addr(url);  
 addr.sin_port = htons(atoi(port));  
  
 if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0 )
 {  
  printf("Connection Error!\n");  
  exit(1);  
 }  
 printf("Connection is successful.\n\n\n");  
 ptr = strtok(path, "/");  
 strcpy(path, ptr);  
 
 // writes the HTTP GET Request to the sockfd  
 write(sockfd, getrequest, strlen(getrequest));  
 return sockfd;  
}  
  
 void openFile() {  
 char * temp;  
 char command[100];  
 char file_Name[1000];  
 strcpy(file_Name, path);   
 if ((temp = strstr(contentFileType, "text/html")) != NULL)
  {  
  if ((temp = strstr(file_Name, ".txt")) != NULL) 
  {  
   sprintf(command, "gedit %s", file_Name);  
  } 
  system(command);  
 } 
 else {  
  printf("This filetype %s is not supported here. Thus, it failed to open %s!\n", contentFileType, file_Name);  
 }  
}   

int parse_header(char * header)
 {  
 char * line, * key, * value;  
 char temp[100];  
 int i = 0;  
 line = strtok(header, "\n");  
 while (line != NULL) {  
  strcpy(temp, line);  
  value = split_key_val(line, i);    
  if (i == 3) {     
   strcpy(contentFileType, value);  
  }  
  line = strtok(NULL, "\n");  
  i++;   
 }  
 for (i = 0; i < 4; i++) {  
  if (status[i] == 0) return 1;  
  }  
 return 0;  
}  
  
char * split_key_val(char * line, int index) {  
 char * temp;  
 if ((temp = strstr(line, keys[index])) != NULL)
  {  
  temp = temp + strlen(keys[index]);  
  status[index] = 1;  
 }  
 return temp;  
}  

int main(int argc, char**argv) {    
 struct sockaddr_in addr, cl_addr;    
 int sockfd, ret;   
 struct hostent * server;  
 char * url, * temp;  
 int port_Num =80;  
 char * file_Name;  
 char status_ok[] = "Okay";  
 char buffer[BUF_SIZE];   
 char http_not_found[] = "HTTP/1.1 404 Not Found";  
 char http_ok[] = "HTTP/1.1 200 Okay";  
 char location[] = "Location: ";  
 char content_type[] = "Content-type: ";  
 int sPos, ePos;  
  
 if (argc < 3) {  
  printf("usage: [URL] [port number]\n");  
  exit(1);    
 }  
  
 url = argv[1];  
 port_Num = atoi(argv[2]);  
  
 //checking the protocol specified  
 if ((temp = strstr(url, "http://")) != NULL) 
 {  
  url = url + 7;  
 } else if ((temp = strstr(url, "https://")) != NULL) {  
  url = url + 8;  
 }  
  
 //checking the port number  
 if (port_Num > 65536 || port_Num < 0)
  {  
  printf("Invalid Port Number!");  
  exit(1);  
 }  
   
 sockfd = get_request(url, argv[2]);   
  
 memset(&buffer, 0, sizeof(buffer));  
 ret = recv(sockfd, buffer, BUF_SIZE, 0);    
 if (ret < 0) 
 {    
  printf("Error receiving HTTP status!\n");      
 } else {  
  printf("%s\n", buffer);  
  if ((temp = strstr(buffer, http_ok)) != NULL) 
  {  
   send(sockfd, status_ok, strlen(status_ok), 0);  
  } else {  
   close(sockfd);  
   return 0;  
  }  
 }  
  
 memset(&buffer, 0, sizeof(buffer));   
 ret = recv(sockfd, buffer, BUF_SIZE, 0);    
 if (ret < 0) 
 {    
  printf("Error receiving HTTP header!\n");      
 } else {  
  printf("%s\n", buffer);
  for(int i=0; i< (argc-1);i++){
    //check if the command contain -h
  if(strcmp("-h",argv[i])==0){
  if (parse_header(buffer) == 0)
   {  
   send(sockfd, status_ok, strlen(status_ok), 0);  
  } else {  
   printf("Error in HTTP header!\n");  
   close(sockfd);  
   return 0;  
  }  
}
}
 }   
  
 fileptr = fopen(path, "w");  
 if (fileptr == NULL) 
 {  
  printf("Error opening file!\n");  
  close(sockfd);  
  return 0;  
 }  
  
 memset(&buffer, 0, sizeof(buffer));  
 while (recv(sockfd, buffer, BUF_SIZE, 0) > 0)
  { 
  //receives the file  
  if ((strstr(contentFileType, "text/html")) != NULL) 
  {  
   fprintf(fileptr, "%s", buffer);  
  } else {  
   fwrite(&buffer, sizeof(buffer), 1, fileptr);  
  }  
  memset(&buffer, 0, sizeof(buffer));  
 }  
 fclose(fileptr);  
 close(sockfd);  
 openFile();  
 return 0;  
}  
