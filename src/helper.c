#include "common.h"

// Get size of file being requested on a socket
off_t get_fsize(int sockfd) {
    char buf[8192], method[8192], uri[812], version[8192];
    char filename[8192], cgiargs[8192];
    struct stat s;
    recv(sockfd, buf, sizeof(buf), MSG_PEEK);
    sscanf(buf, "%s %s %s\n", method, uri, version);
    request_parse_uri(uri, filename, cgiargs);
    stat(filename, &s);
    return (long)s.st_size;
}
