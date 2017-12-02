#ifndef _response_
#define _response_

#include <stdbool.h>

#include "../stream/stream.h"
#include "./request.h"

typedef struct {
  char **    headers;
  char *     headers_buf;
  size_t     num_headers;
  int        status;
  char *     status_message;
  stream_t * body;
} response_response_t;

response_response_t response_resolve(request_request_t req);
void response_close(response_response_t resp);

#endif
