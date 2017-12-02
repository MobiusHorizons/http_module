/*****
 * Simple Restful Get request in c.
 * ****************/


#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
export {
#include <stdbool.h>
}

import closure  from "../closure/closure.module.c";
import stream   from "../stream/stream.module.c";
import net      from "../net/net.module.c";

import url      from "./url.module.c";
import buffered from "./buffered_io.module.c";
import common   from "./common.module.c";
import request  from "./request.module.c";

export typedef struct {
  char **    headers;
  char *     headers_buf;
  size_t     num_headers;
  int        status;
  char *     status_message;
  stream.t * body;
} response_t;

static void * free_buffer(void * buffer, void * args){
  free(buffer);
  return NULL;
}

export response_t resolve(request.request_t req){
  common.params_t p = req.params;
  int error;
  stream.t * out  = buffered.new(req.conn);
  response_t resp = {0};
  resp.body = out;

  char * headers_buf = NULL;
  size_t headers_len = 0;

  char * buf = malloc(4096);
  char * c = buf;

  bool done = false;
  while (!done){
    ssize_t len = stream.read(out, buf, 4096);
    if (len < 0) {
      resp.status = -1;
      free(headers_buf);
      return resp;
    }
    char * body = memmem(buf, len, "\r\n\r\n", 4);

    if (!resp.status){
      const char * status_line = index(buf, ' ');
      resp.status = atol(status_line);
      if (resp.status){
        const char * end = index(status_line, '\n');
        size_t len = end - status_line;
        resp.status_message = strndup(&status_line[1], len);
        resp.status_message[len-1] = 0;
      }
    }

    if (p.keep_headers){
      char * end = body ? body : &(buf[len]);
      headers_buf = realloc(headers_buf, headers_len + (end - buf) + 1);
      memcpy(&headers_buf[headers_len], buf, (end - buf));
      headers_len += (end - buf);
      headers_buf[headers_len] = '\0';
    }

    if (body){
      size_t skip = body - buf + 4;

      buffered.rewind(out, &buf[skip], len - skip, closure.new(free_buffer, buf));
      done = true;
    }
  }

  if (p.keep_headers && headers_len && headers_buf) {
    size_t allocated = 32;
    resp.num_headers    = 0;
    char * iterator = headers_buf;

    resp.headers = malloc(allocated * sizeof(char *));

    do {
      if (resp.num_headers >= allocated){
        allocated *= 2;
        resp.headers = realloc(resp.headers, allocated * sizeof(char *));
      }

      resp.headers[resp.num_headers++] = iterator;
      iterator = strstr(iterator, "\r\n");
      if (iterator){
        iterator[0] = '\0';
        iterator += 2;
      }
    } while(iterator);
  }

  resp.headers_buf = headers_buf;
  resp.body = out;
  return resp;
}

export void close(response_t resp){
  if (resp.num_headers > 0){
    free(resp.headers_buf);
    free(resp.headers);
  }

  stream.t * conn = buffered.wrapped(resp.body);
  if (conn) net.hangup(conn);
  stream.close(resp.body);

}

