#ifndef __REQUEST_H__

void request_handle(int fd);
int request_parse_uri(char *uri, char *filename, char *cgiargs);

#endif // __REQUEST_H__
