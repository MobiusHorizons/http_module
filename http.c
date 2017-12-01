

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "./lib/http.h"
#include "./deps/stream/stream.h"
#include "./deps/stream/file.h"


char * headers[] = {
  "User-Agent: curl/7.54.0",
  NULL
};

struct method_map {
  const char *   name;
  enum common_verb method;
};

struct method_map methods[] = {
  { .name = "GET",  .method = GET  },
  { .name = "PUT",  .method = PUT  },
  { .name = "POST", .method = POST },
  { .name = "HEAD", .method = HEAD },
};

int main(int argc, char ** argv){
  bool verbose = false;
  enum common_verb method = GET;

  char ** arg = argv + 1;
  while( argc > 1 && (*arg)[0] == '-') {
    if (strcmp(*arg, "-v") == 0) {
      verbose = true;
    }
    if (strcmp(*arg, "-m") == 0 && argc > 2) {
      arg++; argc--;
      int m = 0;
      size_t num_methods = sizeof(methods)/sizeof(methods[0]);
      for (m = 0; m < num_methods; m++){
        if (strcmp(*arg, methods[m].name) == 0) break;
      }

      if ( m == num_methods ){
        fprintf(stderr, "Invalid method %s, expecting [GET | PUT | POST | HEAD] \n", *arg);
        return -1;
      }

      method = methods[m].method;
    }
    arg++; argc--;
  }

  if (argc < 2){
    fprintf(stderr, "Usage: %s [-v] [-m METHOD] URL\n", argv[0]);
    return -1;
  }

  common_params_t p = {
    .method       = method,
    .verbose      = verbose,
    .keep_headers = verbose,
    .url          = *arg,
    .headers      = headers,
  };

  request_request_t req = request_request(p);
  if (req.conn->error.code != 0){
    fprintf(stderr, "Failed to connect to %s port %d: %s\n",
        req.url->host,
        req.url->port,
        req.conn->error.message
    );
    return -1;
  }
  if (method == PUT || method == POST) {
    stream_t * body = file_new(0);
    ssize_t error = stream_pipe(body, req.conn);

    stream_write(req.conn, "\r\n", 2);
    if (error < 0){
      return -1;
    }
  }

  response_response_t resp = response_resolve(req);

  if (resp.status == -1) {
    fprintf(stderr, "Error: %s\n", resp.body->error.message);
    return 1;
  }

  if (resp.status < 200 || resp.status >= 300) {
    fprintf(stderr, "The requested URL returned error: %s\n", resp.status_message);
    return 1;
  }

  if (verbose) {
    int i;
    for(i = 0; i < resp.num_headers; i++) {
      printf("< %s\n", resp.headers[i]);
    }
  }

  stream_t * out = file_new(1);
  ssize_t error = stream_pipe(resp.body, out);
  if (error < 0) {
    fprintf(stderr, "Error: %s\n", resp.body->error.message);
    return -1;
  }

  response_close(resp);
}
