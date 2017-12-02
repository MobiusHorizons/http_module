#ifndef _request_
#define _request_

#include "./common.h"
#include "./url.h"
#include "../stream/stream.h"

typedef struct {
  common_params_t  params;
  url_Url *        url;
  stream_t *       conn;
} request_request_t;

request_request_t request_request(common_params_t p);

#endif
