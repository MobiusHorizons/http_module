export {
#include <stdlib.h>
#include <stdbool.h>
}

import stream from "../stream/stream.module.c";
import file   from "../stream/file.module.c";

export enum verb {
  UNDEFINED_METHOD = 0,
  GET,
  PUT,
  POST,
  HEAD,
};

export static const char * method_names[] = {
  "UNDEFINED",
  "GET",
  "PUT",
  "POST",
  "HEAD",
};

export typedef struct {
  enum verb    method;
  const char * url;
  char **      headers;
  bool         keep_headers;
  bool         verbose;
} params_t;

export extern const char * HTTP_VERSION;
const char * HTTP_VERSION = "HTTP/1.0";

export ssize_t read(stream.t * in, void * buf, size_t len, bool is_verbose){
  ssize_t read_bytes = stream.read(in, buf, len);
  stream.t * err_stream = file.new(2);
  if (is_verbose && read_bytes > 0){
    stream.write(err_stream, "< ", 2);
    stream.write(err_stream, buf, read_bytes);
  }
  return read_bytes;
}

export ssize_t write(stream.t * out, const void * buf, size_t len, bool is_verbose){
  ssize_t written_bytes = stream.write(out, buf, len);
  stream.t * err_stream = file.new(2);
  if (is_verbose && written_bytes > 0){
    stream.write(err_stream, "> ", 2);
    stream.write(err_stream, buf, written_bytes);
  }
  return written_bytes;
}
