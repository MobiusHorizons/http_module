#define BUF_SIZE 1024
#include <stdlib.h>
#include <string.h>

#include "../deps/closure/closure.h"
#include "../deps/stream/stream.h"
static int _type;

int buffered_io_type() {
  if (_type == 0) {
    _type = stream_register("file");
  }

  return _type;
}

typedef struct buffered_t buffered_io_Buffered;
struct buffered_t {
  char *      buffer;
  size_t      buffered;
  size_t      cursor;
  stream_t * _wrapped;
  closure_t free_fn;
};

static ssize_t read(void * ctx, void * buffer, size_t nbytes, stream_error_t * error) {
  buffered_io_Buffered * b = (buffered_io_Buffered *) ctx;
  ssize_t len = 0;

  if (b->buffer) {
    size_t remaining = b->buffered - b->cursor;
    len = remaining > nbytes ? nbytes : remaining;
    memcpy(buffer, &(b->buffer[b->cursor]), len);
    nbytes -= len;
    b->cursor += len;

    if (b->cursor == b->buffered) {
      b->cursor = b->buffered = 0;
      closure_call(b->free_fn, b->buffer);
      b->buffer = NULL;
    }
  }

  if (nbytes > 0){
    ssize_t err = stream_read(b->_wrapped, &buffer[len], nbytes);
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
static ssize_t write(void * ctx, const void * buffer, size_t nbytes, stream_error_t * error) {
  buffered_io_Buffered * b = (buffered_io_Buffered *) ctx;
  ssize_t e = stream_write(b->_wrapped, buffer, nbytes);
  if (e < 0 && error != NULL) {
      error->code    = b->_wrapped->error.code;
      error->message = b->_wrapped->error.message;
  }

  return e;
}

static ssize_t close(void * ctx, stream_error_t * error) {
  buffered_io_Buffered * b = (buffered_io_Buffered *) ctx;

  if (b && b->buffered > 0){
    closure_call(b->free_fn, b->buffer);
  }

  free(b);
  return 0;
}

stream_t * buffered_io_new(stream_t * _wrapped){
  buffered_io_Buffered *b = malloc(sizeof(buffered_io_Buffered));
  b->_wrapped = _wrapped;
  b->cursor   = 0;
  b->buffered = 0;
  b->buffer   = NULL;
  b->free_fn  = closure_new(NULL, NULL);

  stream_t * s = malloc(sizeof(stream_t));

  s->ctx   = b;
  s->read  = read;
  s->write = write;
  s->pipe  = NULL;
  s->close = close;
  s->type = buffered_io_type();

  s->error.code    = 0;
  s->error.message = NULL;

  return s;
}

size_t buffered_io_rewind(stream_t * s, void * buffer, size_t bytes, closure_t free_fn){
  if (s->type == buffered_io_type()){
    buffered_io_Buffered * b = (buffered_io_Buffered *) s->ctx;
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

stream_t * buffered_io_wrapped(stream_t * s){
  if (s->type == buffered_io_type()){
    buffered_io_Buffered * b = (buffered_io_Buffered *) s->ctx;
    if (b) {
      return b->_wrapped;
    }
  }
  return NULL;
}
