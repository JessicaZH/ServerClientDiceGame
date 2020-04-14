#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <wait.h>

void servicePlayers (int sd1,  int sd2);
//void servicePlayers (int sd1);
int main(int argc, char *argv[]){
    
    int sd, client1, client2, portNumber;
    socklen_t len;
    struct sockaddr_in servAdd;

    if (argc != 2){
        printf("Call model: %s <Port #>\n", argv[0]);
        exit(0);
    }

    if ((sd=socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1);
    }

    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr=htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port=htons((uint16_t)portNumber);
    bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));
    listen(sd,5);

    while(1){
        client1=accept(sd, (struct sockaddr*)NULL,NULL);
        printf("Got a client\n");
        printf("Waiting for another client to start the game\n");
        while(1){
            client2=accept(sd,(struct sockaddr*)NULL, NULL);
            printf("Got another client\n");
            break;
        }
        if (!fork())
            servicePlayers(client1,client2);
        close(client1);
        close(client2);
    }
}

void servicePlayers (int sd1,int sd2){
    int point[2]={0,0};
    char message[255];
    int dice;

    while(1){
        
        //client1
        write(sd1, "You can now play", 18);
        if (!read(sd1,message,255)){
            close(sd1);
            fprintf(stderr, "client 1 quit, game over\n");
            exit(0);
        }
        sscanf(message,"%d",&dice);
        fprintf(stderr, "client 1 send back: %d\n", dice);
        point[0]+=dice;
        fprintf(stderr, "client 1 %d point so far\n",point[0]);
        
        sleep(2);
        if (point[0]>=100)
            break;

        //client2
        write(sd2,"You can now play",18);
        if (!read(sd2,message, 2)){
            close(sd2);
            fprintf(stderr,"client 2 quit, game over\n");
            exit(0);
        }
        sscanf(message,"%d",&dice);
        fprintf(stderr,"client 2 send back: %d\n",dice);
        point[1]+=dice;
        fprintf(stderr, "client 2 %d point so far\n",point[1]);
        sleep(2);
        
        if (point[1]>=100)
            break;
        
    }
    //client1
    if(point[0]>=100){
        write(sd1,"Game over: you won the game\n",29);
        close(sd1);
    }else{
        write(sd1,"Game over: you lost the game\n",30);
        close(sd1);
    }
    //client2
    if (point[1]>=100){
        write(sd2,"Game over: you won the game\n",29);
        close(sd2);
    }else{
        write(sd2,"Game over: you lost the game\n",30);
        close(sd2);
    }
    exit(0);
}