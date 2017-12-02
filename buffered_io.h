#ifndef _buffered_io_
#define _buffered_io_

#include "../stream/stream.h"
#include "../closure/closure.h"

int buffered_io_type();

typedef struct buffered_t buffered_io_Buffered;

stream_t * buffered_io_new(stream_t * _wrapped);
size_t buffered_io_rewind(stream_t * s, void * buffer, size_t bytes, closure_t free_fn);
stream_t * buffered_io_wrapped(stream_t * s);

#endif
