#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

char* r200 ="HTTP/1.1 200 OK\r\n\r\n";
char* r400 ="HTTP/1.1 400 Not Found\r\n\r\n";

char sec_buf[4096];
char method[30];
char uri[4096];
char ver[90];
char buf[4096];
char res[4096];
char file_cont[4096];



void response(char *status, char *cont_type, int cont_len, char *body) {
    sprintf(res, "%s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", status, cont_type, cont_len ,body);
}

void file_sender(int sockfd){
   struct stat file;

   char* filepath = strstr(uri, "/files/" );
   char *filename = filepath + 7;

   int filefd = open(filename,O_RDONLY, S_IRWXU );
   if(filefd == -1){
       write(sockfd, r400, strlen(r400));
       return ;
   }
   stat(filename, &file);
   int bytrd = read(filefd, file_cont,file.st_size );

    response("HTTP/1.1 200 OK","application/octet-stream",strlen(file_cont) , file_cont);
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
        pars_req_line(sockfd);
        if((strcmp(uri, "/")) == 0) {
            write(sockfd, r200, strlen(r200));
        }else if((strncmp(uri, "/echo/",6) == 0)){
            int len = strlen(uri);
            char *val = uri + 6;
            response("HTTP/1.1 200 OK","text/plain", strlen(val), val);
            write(sockfd, res, strlen(res));

        }
        else if((strcmp(uri, "/user-agent")) == 0){
            header_parser(sockfd);

        }else if((strncmp(uri, "/files",5)) == 0){
            file_sender(sockfd);

            write(sockfd, res, strlen(res));
        }
        else{
            write(sockfd, r400, strlen(r400));
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

        process(clientfd);
    }

    close(sockfd);
    return 0;
}
