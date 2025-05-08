#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>



void process(int sockfd){
    char buf[4096];
    char* res = "HTTP/1.1 200 OK\r\n\r\nHeeelooo";
    int b_readed = read(sockfd, buf, sizeof(buf));
    if(b_readed>0)
     write(sockfd,res, strlen(res));
   close(sockfd) ;
}


int main(){
    printf("server starting ...");
    struct sockaddr_in server_addr, client_addr;
    int opt =1;
    int clientfd;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1 ){
        fprintf(stderr,"socket func err");
        return 1;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8181);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t addrlen = sizeof(server_addr);
    socklen_t clientsize = sizeof(client_addr);

    if((bind(sockfd,(struct sockaddr*)&server_addr, addrlen )) != 0){
        fprintf(stderr,"bind func err");
        return 1;
    }
    if((listen(sockfd, 5)) != 0){
        fprintf(stderr,"listen func err");
        return 1;
    }

    while(1){
        clientfd = accept(sockfd,(struct sockaddr*)&client_addr, &clientsize );
       if(clientfd == -1){
           fprintf(stderr,"accept err");
           return 1;
       }

       process(clientfd);
    }
    close(sockfd);
    close(clientfd);

}
