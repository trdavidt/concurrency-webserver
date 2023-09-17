#include "common.h"

#define WSERVER_USAGE "Usage: wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s SFF/FIFO]\n"

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = DEFAULT_ROOT;
    int port = DEFAULT_PORT;
    int num_threads = DEFAULT_THREADS;
    int capacity = DEFAULT_BUFFERS;
    char *policy = DEFAULT_POLICY;
    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1) {
        switch (c) {
            case 'd':
                root_dir = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                num_threads = atoi(optarg);
                break;
            case 'b':
                capacity = atoi(optarg);
                break;
            case 's':
                if (strcmp(optarg, FIFO) == 0) {
                    policy = FIFO;
                }
                else if (strcmp(optarg, SFF) == 0) {
                    policy = SFF;
                }
                else {
                    printf("%s\n", optarg);
                    fprintf(stderr, WSERVER_USAGE);
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, WSERVER_USAGE);
                exit(1);
            }
    } 

    printf("Starting server... ");

    // init scheduler and thread pool
    scheduler *s = init_scheduler(policy, capacity);
    thread_pool *workers = init_thread_pool(num_threads);
    create_workers(s, workers);
	
    // run out of this directory
    chdir_or_die(root_dir);

    struct sockaddr_in client_addr;
    int client_len;
    int sockfd;

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    printf("listening for connections\n");
    while (1) {
        client_len = sizeof(client_addr);
        sockfd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr,
                                    (socklen_t *) &client_len);
        scheduler_post(workers, s, sockfd);
    }
    return 0;
}

