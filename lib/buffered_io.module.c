#define BUF_SIZE 1024
#include <stdlib.h>
#include <string.h>

import closure  from "../deps/closure/closure.module.c";
import stream   from "../deps/stream/stream.module.c";
static int _type;

export int type() {
  if (_type == 0) {
    _type = stream.register("file");
  }

  return _type;
}

export typedef struct buffered_t Buffered;
struct buffered_t {
  char *      buffer;
  size_t      buffered;
  size_t      cursor;
  stream.t * _wrapped;
  closure.t free_fn;
};

static ssize_t read(void * ctx, void * buffer, size_t nbytes, stream.error_t * error) {
  Buffered * b = (Buffered *) ctx;
  ssize_t len = 0;

  if (b->buffer) {
    size_t remaining = b->buffered - b->cursor;
    len = remaining > nbytes ? nbytes : remaining;
    memcpy(buffer, &(b->buffer[b->cursor]), len);
    nbytes -= len;
    b->cursor += len;

    if (b->cursor == b->buffered) {
      b->cursor = b->buffered = 0;
      closure.call(b->free_fn, b->buffer);
      b->buffer = NULL;
    }
  }

  if (nbytes > 0){
    ssize_t err = stream.read(b->_wrapped, &buffer[len], nbytes);
    if (err < 0 && error != NULL) {
      error->code    = b->_wrapped->error.code;
      error->message = b->_wrapped->error.message;
      return err;
    }
    len += err;
  }

  return len;
}

// passthrough
static ssize_t write(void * ctx, const void * buffer, size_t nbytes, stream.error_t * error) {
  Buffered * b = (Buffered *) ctx;
  ssize_t e = stream.write(b->_wrapped, buffer, nbytes);
  if (e < 0 && error != NULL) {
      error->code    = b->_wrapped->error.code;
      error->message = b->_wrapped->error.message;
  }

  return e;
}

static ssize_t close(void * ctx, stream.error_t * error) {
  Buffered * b = (Buffered *) ctx;

  if (b && b->buffered > 0){
    closure.call(b->free_fn, b->buffer);
  }

  global.free(b);
  return 0;
}

export stream.t * new(stream.t * _wrapped){
  Buffered *b = malloc(sizeof(Buffered));
  b->_wrapped = _wrapped;
  b->cursor   = 0;
  b->buffered = 0;
  b->buffer   = NULL;
  b->free_fn  = closure.new(NULL, NULL);

  stream.t * s = malloc(sizeof(stream.t));

  s->ctx   = b;
  s->read  = read;
  s->write = write;
  s->pipe  = NULL;
  s->close = close;
  s->type = type();

  s->error.code    = 0;
  s->error.message = NULL;

  return s;
}

export size_t rewind(stream.t * s, void * buffer, size_t bytes, closure.t free_fn){
  if (s->type == type()){
    Buffered * b = (Buffered *) s->ctx;
    if (b->buffer == NULL) {
      b->buffer   = buffer;
      b->free_fn  = free_fn;
      b->cursor   = 0;
      b->buffered = bytes;
      return bytes;
    }
    return 0;
  }
  return -1;
}

export stream.t * wrapped(stream.t * s){
  if (s->type == type()){
    Buffered * b = (Buffered *) s->ctx;
    if (b) {
      return b->_wrapped;
    }
  }
  return NULL;
}
