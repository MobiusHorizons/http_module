#ifndef _common_
#define _common_

#include <stdlib.h>
#include <stdbool.h>

#include "../deps/stream/stream.h"

enum common_verb {
  UNDEFINED_METHOD = 0,
  GET,
  PUT,
  POST,
  HEAD,
};

static const char * common_method_names[] = {
  "UNDEFINED",
  "GET",
  "PUT",
  "POST",
  "HEAD",
};

typedef struct {
  enum common_verb    method;
  const char * url;
  char **      headers;
  bool         keep_headers;
  bool         verbose;
} common_params_t;

extern const char * common_HTTP_VERSION;
ssize_t common_read(stream_t * in, void * buf, size_t len, bool is_verbose);
ssize_t common_write(stream_t * out, const void * buf, size_t len, bool is_verbose);

#endif
