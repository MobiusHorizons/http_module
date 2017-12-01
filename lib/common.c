
#include <stdlib.h>
#include <stdbool.h>


#include "../deps/stream/stream.h"
#include "../deps/stream/file.h"

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


const char * common_HTTP_VERSION = "HTTP/1.0";

ssize_t common_read(stream_t * in, void * buf, size_t len, bool is_verbose){
  ssize_t read_bytes = stream_read(in, buf, len);
  stream_t * err_stream = file_new(2);
  if (is_verbose && read_bytes > 0){
    stream_write(err_stream, "< ", 2);
    stream_write(err_stream, buf, read_bytes);
  }
  return read_bytes;
}

ssize_t common_write(stream_t * out, const void * buf, size_t len, bool is_verbose){
  ssize_t written_bytes = stream_write(out, buf, len);
  stream_t * err_stream = file_new(2);
  if (is_verbose && written_bytes > 0){
    stream_write(err_stream, "> ", 2);
    stream_write(err_stream, buf, written_bytes);
  }
  return written_bytes;
}
