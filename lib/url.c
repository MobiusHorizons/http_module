
#include <sys/types.h>
#include <stdbool.h>


#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

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

enum parse_states {
  scheme_start = 0,
  scheme_main,
  scheme_delim1,
  scheme_delim2,
  host,
  port_s,
  route,
  qs_key,
  qs_val,
  hash,

  error_state
};

url_Url * url_parse(const char * in, int * error){
  url_Url * u = malloc(sizeof(url_Url));
  int params_count = 0;

  u->host      = NULL;
  u->host_port = NULL;
  u->route     = NULL;
  u->hash      = NULL;
  u->qs        = NULL;
  u->is_tls    = false;

  char * url = strdup(in);
  char * scheme = url;
  char * port = NULL;
  char * qs = NULL;
  char * cursor = url;
  u->_mem = url;

  enum parse_states state = 0;
  while(cursor != NULL && *cursor != '\0'){
    switch(state){

      case scheme_start:
        if (isalpha(*cursor)) {
          state = scheme_main;
          break;
        }
        *error = -1;
        free(u);
        return NULL;

      case scheme_main:
        if (*cursor == ':'){
          state = scheme_delim1;
          *cursor = '\0';
          break;
        }

        if (!(isalpha(*cursor) || isdigit(*cursor) || *cursor == '+' || *cursor == '-' || *cursor == '.')){
          *error = -2;
          free(u);
          return NULL;
        }
        break;

      case scheme_delim1:
      case scheme_delim2:
        if (*cursor == '/'){
          state++;
          break;
        }
        *error = -3;
        free(u);
        return NULL;

      case host:
      case port_s:
      case route:
        if (u->host == NULL) u->host = cursor;

        if (state < port_s && *cursor == ':'){
          *cursor = '\0';
          port = cursor+1;
          state = port_s;
          break;
        }
        if (state < route && *cursor == '/'){
          *cursor = '\0';
          u->route = cursor+1;
          state = route;
          break;
        }
        if (state < qs_key && *cursor == '?'){
          *cursor = '\0';
          qs = cursor + 1;
          state = qs_key;
          break;
        }
        if (*cursor == '#'){
          *cursor = '\0';
          u->hash = cursor + 1;
          state = hash;
          break;
        }
        break;

      case qs_key:
        if (*cursor == '=') {
          *cursor = '\0';
          params_count++;
          state = qs_val;
          break;
        }

        if (*cursor == '#'){
          *cursor = '\0';
          u->hash = cursor + 1;
          state = hash;
          break;
        }
        break;

      case qs_val:
        if (*cursor == '&'){
          *cursor = '\0';
          state = qs_key;
          break;
        }

        if (*cursor == '#'){
          *cursor = '\0';
          u->hash = cursor + 1;
          state = hash;
          break;
        }
        break;

      case hash:
      case error_state:
        break;
    }
    cursor++;
  }
  char * end = cursor;
  u->_end = end;

  /* scheme validation + port */
  if (scheme == NULL){
    *error = -1;
    free(u);
    return NULL;
  }
  if (strncasecmp(scheme, "http", 4) == 0){
    if (scheme[4] == '\0'){
      u->port = 80;
    } else if (strcasecmp(&scheme[4], "s") == 0){
      u->port = 443;
      u->is_tls = true;
    } else {
      *error = -4;
      free(u);
      return NULL;
    }
  } else {
    *error = -4;
    free(u);
    return NULL;
  }

  if (port){
    int port_num = (int)atol(port);
    if (u->port != port_num){
      u->host_port = malloc(strlen(u->host) + strlen(port) + 2);
      strcpy(u->host_port, u->host);
      strcat(u->host_port, ":");
      strcat(u->host_port, port);
    }
    u->port = port_num;
  }

  //TODO: leave out the \0 on the first pass.
  //Add them into the original.

  if (qs != NULL){
    u->qs = strdup(qs);
    if (qs && params_count > 0){
      u->params = malloc(sizeof(url_parameter) * (params_count + 1));
      bool kv = 0;
      char * c = qs;
      int i = 0;
      while (c < (u->hash ? u->hash : end)){
        u->params[i][kv] = c;
        while(c < (u->hash ? u->hash : end) && c != NULL && *c != '\0') c++;
        c++;
        kv = !kv;
        if (kv == false){
          i++;
        }
      }
      u->params[params_count][0] = NULL;
    }
  }

  return u;
}

char * url_query_string(url_Url * u){
  if (u->qs == NULL) return "";

  return u->qs;
}

void url_free(url_Url * u){
  if (u){
    free(u->host_port);
    free(u->_mem);
    free(u);
  }
}
