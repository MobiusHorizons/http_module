#ifndef _url_
#define _url_

#include <sys/types.h>
#include <stdbool.h>

typedef char * url_parameter [2];
typedef struct {
  char      * host;
  char      * route;
  char      * qs;
  char      * host_port;
  url_parameter * params;
  char      * hash;
  int         port;
  bool        is_tls;
  char      * _mem;
  char      * _end;
} url_Url;

url_Url * url_parse(const char * in, int * error);
char * url_query_string(url_Url * u);
void url_free(url_Url * u);

#endif
