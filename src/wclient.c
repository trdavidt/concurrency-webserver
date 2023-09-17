#include "common.h"

#define WCLIENT_USAGE "Usage: wclient <host> <port> <file path(s)>\n"

//
// Send an HTTP request for the specified file 
//
void client_send(int fd, char *filename) {
    char buf[MAXBUF];
    char hostname[MAXBUF];
    char hostheader[MAXBUF];
    
    gethostname_or_die(hostname, MAXBUF);
    
    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(hostheader, "host: %s\n\r\n", hostname);
    strncat(buf, hostheader, MAXBUF);
    write_or_die(fd, buf, strlen(buf));
}

//
// Read the HTTP response and print it out
//
void client_print(int fd) {
    char buf[MAXBUF];  
    int n;
    
    // Read and display the HTTP Header 
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
        printf("Header: %s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
    }
    
    // Read and display the HTTP Body 
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
        printf("%s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
    }
}

// 
// Client worker thread opens a connection to server
//
void *client_routine(void *arg) {
    
    client_arg *c = (client_arg *)(arg);
    char *host = c->host;
    char *fname = c->fname;
    int port = c->port;
    int clientfd = open_client_fd_or_die(host, port);

    client_send(clientfd, fname);
    client_print(clientfd);
    close_or_die(clientfd);

    return NULL;
}

int main(int argc, char *argv[]) {
    char *host;
    int port;
    
    if (argc < 4) {
        fprintf(stderr, WCLIENT_USAGE);
        exit(1);
    }
    
    host = argv[1];
    port = atoi(argv[2]);

    // Total num of files
    int num_clients = argc - 3;
    pthread_t client_threads[num_clients];

    // Create client threads to make rquests for each file
    for (int i = 0; i < num_clients; i++) {
        client_arg *c = (client_arg *)malloc(sizeof(client_arg));
        if (!c) {
            printf("main: could not create request for %s\n", argv[3+i]);
            continue;
        }
        c->host = host;
        c->port = port;
        c->fname = argv[3+i];
        pthread_create(&client_threads[i], NULL, client_routine, (void *)c);
    }

    // Join client threads to main thread
    for (int i = 0; i < num_clients; i++) {
        pthread_join(client_threads[i], NULL);
    }

    exit(0);
}
