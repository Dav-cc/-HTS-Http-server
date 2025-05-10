#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

char* r200 ="HTTP/1.1 200 OK\r\n\r\n";

char* r404 ="HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: 13\r\n"
             "\r\n"
             "404 Not Found";

char sec_buf[4096];
char method[30];
char uri[4096];
char ver[90];
char buf[4096];
char res[4096];
char file_cont[4096] = {0};


void response(char *status, char *cont_type, int cont_len, char *body) {
    sprintf(res, "%s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", status, cont_type, cont_len ,body);
}

int file_sender(int sockfd){
   struct stat file;

   char* filepath = strstr(uri, "/files/" );
   char *filename = filepath + 7;

   int filefd = open(filename,O_RDONLY, S_IRWXU );
   if(filefd <=0 ){
       write(sockfd, r404, strlen(r404));
       close(sockfd);
       return 1 ;
   }
   int st = stat(filename, &file);
   if(st == -1){
       write(sockfd, r404, strlen(r404));
       printf("error i stat func");
       close(sockfd);
       return 1;
   }
   int bytrd = read(filefd, file_cont,file.st_size );
   if(bytrd <= 0){
       printf("error in reading file");
       write(sockfd, r404, strlen(r404));
       close(sockfd);
       return 1;
   }

    response("HTTP/1.1 200 OK","application/octet-stream", bytrd, file_cont);
    return 0;
}

void header_parser(int sockfd){
    char *det = strtok(sec_buf, "\r\n");
    char *value;
    while(det != NULL){
        if((strncmp(det, "User-Agent",10)) == 0){
            value = det+11;
        }
            det = strtok(NULL, "\r\n");

    }
    int userlen = strlen (value);
    response("HTTP/1.1 200 OK","text/plain",userlen , value);
    write(sockfd, res, strlen(res));


}


void pars_req_line(int sockfd){
    int b_readed = read(sockfd, buf, sizeof(buf));
    strcpy(sec_buf, buf);
    if (b_readed > 0) {
        buf[b_readed]= '\0';
        char *line = strtok(buf, "\r\n");
        sscanf(line, "%s %s %s", method, uri, ver);
}
}

void process(int sockfd) {
        pars_req_line(sockfd); // init method, uri and version of http request

        if((strcmp(uri, "/")) == 0) {
            write(sockfd, r200, strlen(r200));
        }else if((strncmp(uri, "/echo/",6) == 0)){
            char *val = uri + 6;
            response("HTTP/1.1 200 OK","text/plain", strlen(val), val);
            write(sockfd, res, strlen(res));

        }
        else if((strcmp(uri, "/user-agent")) == 0){
            header_parser(sockfd);

        }else if((strncmp(uri, "/files",5)) == 0){
            if(!file_sender(sockfd)){
                write(sockfd, res, strlen(res));
            }
            memset(res, 0, sizeof(res));
        }
        else{
            write(sockfd, r404, strlen(r404));
        }

}


int main() {
    printf("\033[H\033[J");
    printf("server starting ...\n");

    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int clientfd;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "socket func err\n");
        return 1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "setsockopt error\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8181);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t addrlen = sizeof(server_addr);
    socklen_t clientsize = sizeof(client_addr);

    if ((bind(sockfd, (struct sockaddr*)&server_addr, addrlen)) != 0) {
        fprintf(stderr, "bind func err\n");
        return 1;
    }
    printf("binding to port 8181");

    if ((listen(sockfd, 5)) != 0) {
        fprintf(stderr, "listen func err\n");
        return 1;
    }

    while (1) {
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &clientsize);
        if (clientfd == -1) {
            fprintf(stderr, "accept err\n");
            continue;
        }
        printf("connection accepted");

        process(clientfd);
    }

    close(sockfd);
    return 0;
}
