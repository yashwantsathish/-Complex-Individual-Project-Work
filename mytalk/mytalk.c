/*My name is Yashwant Sathish Kumar and this is mytalk.c */

#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <poll.h>
#include <talk.h>
#include <pwd.h>
#include <ctype.h>

#define MAX_CONNECTIONS 1
#define MAXLINE 1024
#define LOCAL 0
#define REMOTE 1

int done;
int has_v = 0;
int has_a = 0;
int has_N = 0;
int sockfd;
struct sockaddr_in sa;
struct hostent *hostent;
char* hostname;


/* Implements functionality to talk back and forth */
void start_comms(int s){

    int i = 0;

    int count;
    char buff[MAXLINE] = {'\0'};
    struct pollfd fds[2];

    fds[LOCAL].fd = STDIN_FILENO;
    fds[LOCAL].events = POLLIN;
    fds[LOCAL].revents = 0;
    fds[REMOTE].fd = s;
    fds[REMOTE].events = POLLIN;
    fds[REMOTE].revents = 0;

    while (1) {
        poll(fds, 2, -1);
        update_input_buffer();
        /*Send lines*/
        if (has_whole_line()) {
            for (i = 0; i < MAXLINE; i++) {
                buff[i] = '\0';
            }
            if ((count = read_from_input(buff, MAXLINE)) > 0) {
                if (has_hit_eof()) {
                    send(s, buff, count, 0);
                    stop_windowing();
                    close(s);
                    exit(0);
                }
                else {
                    send(s, buff, count, 0);
                }
            }
        }
        /* Recieve lines*/
        if (fds[REMOTE].revents & POLLIN) {
            for (i = 0; i < MAXLINE; i++) {
                buff[i] = '\0';
            }
            if ((count = recv(s, buff, MAXLINE, 0)) > 0 && !has_hit_eof()) {
                write_to_output(buff, count);
            }
            else {
                fprint_to_output("\nConnection closed. ^C to terminate.\n");
                close(s);
                while (1) { pause(); }
            }
        }
    }

}

/* Implements client functionality. Established connections
 * and calls start_comms*/
void client(int port, char* hostname){

    uid_t uid;
    struct passwd *user;
    char buf[MAXLINE] = {'\0'};
    char *pass= "ok";
    int i;
    char temphostname[MAXLINE] = {'\0'};
    struct hostent* temphostent; 

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = *(uint32_t*) hostent -> h_addr_list[0];
  
    /* establish connection*/
    if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
  
    if (sprintf(buf, "Waiting for response from %s.\n", hostname) < 0) {
        perror("sprintf");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* write the username + hostname */
    write_to_output(buf, strlen(buf));
    for (i = 0; i < MAXLINE; i++) {
        buf[i] = '\0';
    }

    uid = getuid();
    user = getpwuid(uid);
 
    if((gethostname(temphostname, MAXLINE)) == - 1){
        perror("gethostname");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    temphostent = gethostbyname(temphostname);
   
    if(temphostent == NULL){
        perror("gethostbyname");
        close(sockfd);
        exit(EXIT_FAILURE);
    } 

    if(sprintf(buf, "%s@%s", user -> pw_name, temphostent -> h_name) < 0){
        perror("sprintf");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (send(sockfd, buf, strlen(buf), 0) == -1) {
        stop_windowing();
        perror("send");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < MAXLINE; i++){
        buf[i] = '\0';
    } 


    if (recv(sockfd, buf, MAXLINE, 0) < 0) {
        perror("recv");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (strcmp(buf, pass)) {
        fprintf(stderr, "%s declined connection.\n", hostname);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (!has_N) { start_windowing(); }

    start_comms(sockfd);

}

/* Implements server functionality and calls start_comms */
void server(int port){

    int input, i;
    struct sockaddr_in peerinfo;
    socklen_t slen;
    char client[MAXLINE] = {'\0'};
    char in[4] = {'\0'};
    char *pass = "ok";
    char *fail = "fail";

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /* binds created socket with the port */
    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* "listens" for a new connection */
    if (listen(sockfd, MAX_CONNECTIONS) == -1) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    slen = sizeof(peerinfo);
    while ((input = accept(sockfd, (struct sockaddr *)&peerinfo, &slen)) <= 0) 
    {    }

    /* recieve username and output */
    recv(input, client, MAXLINE, 0);
    
    printf("Mytalk request from %s. Accept (y/n)?\n", client);
   
    /* check to see if yes or no written */
    if (!has_a) {
        scanf("%s", in);

        for (i = 0; i < strlen(in); i++) {
            in[i] = tolower((unsigned char)in[i]);
        }

        if (!strcmp(in, "y") || !strcmp(in, "yes")) {
            if (send(input, pass, strlen(pass), 0) == -1) {
                perror("send");
                close(sockfd);
                exit(1);
            }
        }
        else {
            if (send(input, fail, strlen(fail), 0) == -1) {
                perror("send");
                close(sockfd);
                exit(1);
            }
        }
    }
    else {
        if (send(input, pass, strlen(pass), 0) == -1) {
            perror("send");
            close(sockfd);
            exit(1);
        }
    }

    if (!has_N) { start_windowing(); }

    start_comms(input);
}

/*main takes in the port and hostname as arguments and calls the 
 *  * respect server and client functions depending on the args passed
 *   * in. */
int main(int argc, char *argv[]){

    int opt;
    int port;

    if (argc < 2 || argc > 6) {
        fprintf(stderr, "usage: mytalk [ -v ] [ -a ] [ -N ] [ hostname ] port");
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "vaN")) != -1) {
        switch (opt)
        {
            case 'v':
                has_v = 1;
                break;
            case 'a':
                has_a = 1;
                break;
            case 'N':
                has_N = 1;
                break;
            default:
                fprintf(stderr, 
                "usage: mytalk [ -v ] [ -a ] [ -N ] [ hostname ] port");
                exit(EXIT_FAILURE);
        }
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (optind + 1 == argc) {
        /* call server function */
        port = atoi(argv[optind]);
        if (port < 1024 || port > 65535) {
            fprintf(stderr, "invalid port");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        server(port);
    }

    else if (optind + 2 == argc) {
        /* call client function */
        port = atoi(argv[optind + 1]);
        if (port < 1024 || port > 65535) {
            fprintf(stderr, "invalid port");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
  
        if ((hostent = gethostbyname(argv[optind])) == NULL) {
            perror("hostname");
            close(sockfd);
            exit(1);
        }
    
        client(port, argv[optind]);

    }

    else {
        fprintf(stderr, "usage: mytalk [ -v ] [ -a ] [ -N ] [ hostname ] port");
        exit(EXIT_FAILURE);
    }

    return 0;
}
