#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <wait.h>


int main(int argc, char* argv[]){
    char message[255];
    int server, portNumber;
    socklen_t len;
    struct sockaddr_in servAdd;
    int dice;
    long int ss=0;
    srand((unsigned int)time(NULL)%getpid());

    if(argc != 3){
        printf("Call model: %s <IP> <Port#>\n",argv[0]);
        exit(0);
    }
    if ((server = socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1);
    }

    servAdd.sin_family=AF_INET;
    sscanf(argv[2],"%d",&portNumber);
    servAdd.sin_port=htons((uint16_t)portNumber);

    if (inet_pton(AF_INET,argv[1], &servAdd.sin_addr)<0){
        fprintf(stderr,"inet_port() has failed\n");
        exit(2);
    }
    if(connect(server, (struct sockaddr *)&servAdd, sizeof(servAdd))<0){
        fprintf(stderr, "connect() has failed, exiting\n");
        exit(3);
    }

    while(1){
        //get message from server
        if(read(server, message, 255)<0){
            fprintf(stderr, "read() error\n");
            exit(3);
        }
        fprintf(stderr,"Server's message: %s\n", message);

        //dice and send back point
        if(!strcasecmp(message,"Game over: you won the game\n"))   // won the game
        {
            close(server);
            fprintf(stderr,"I won the game\n");
            exit(0);
        } else if (!strcasecmp(message,"Game over: you lost the game\n"))  //lost the game
        {
            close(server);
            fprintf(stderr,"I lost the game\n");
            exit(0);
        } else
        {
            fprintf(stderr,"I am playing my dice\n");
            dice=rand()%10+1;
            fprintf(stderr,"I got %d points\n",dice);
            //fprintf(stderr,"%d\n",dice);
            sprintf(message,"%d",dice);
            write(server,message,strlen(message)+1);
        }
    }
}