# dependencies for ./http.c
./http.o: ./http.c ./lib/http.h ./deps/stream/stream.h ./deps/stream/file.h

# dependencies for ./lib/http.c
./lib/http.o: ./lib/http.c ./lib/common.h ./lib/request.h ./lib/response.h

# dependencies for ./lib/common.c
./lib/common.o: ./lib/common.c ./deps/stream/stream.h ./deps/stream/file.h ./deps/stream/stream.h

# dependencies for ./deps/stream/stream.c
./deps/stream/stream.o: ./deps/stream/stream.c

# dependencies for ./deps/stream/file.c
./deps/stream/file.o: ./deps/stream/file.c ./deps/stream/stream.h ./deps/stream/stream.h

# dependencies for ./lib/request.c
./lib/request.o: ./lib/request.c ./lib/url.h ./deps/net/net.h ./deps/stream/stream.h ./lib/common.h ./lib/common.h ./lib/url.h ./deps/stream/stream.h

# dependencies for ./lib/url.c
./lib/url.o: ./lib/url.c

# dependencies for ./deps/net/net.c
./deps/net/net.o: ./deps/net/net.c ./deps/stream/stream.h ./deps/net/socket.h ./deps/net/tlssocket.h ./deps/stream/stream.h

# dependencies for ./deps/net/socket.c
./deps/net/socket.o: ./deps/net/socket.c ./deps/stream/stream.h ./deps/stream/file.h ./deps/stream/stream.h

# dependencies for ./deps/net/tlssocket.c
LIBRESSL_INCLUDE_DIR ?= /usr/local/opt/libressl/include
LIBRESSL_LIB_DIR ?= /usr/local/opt/libressl/lib
CFLAGS += -I$(LIBRESSL_INCLUDE_DIR)
LDFLAGS += -L$(LIBRESSL_LIB_DIR) -ltls 
./deps/net/tlssocket.o: ./deps/net/tlssocket.c ./deps/stream/stream.h ./deps/stream/stream.h

# dependencies for ./lib/response.c
./lib/response.o: ./lib/response.c ./deps/closure/closure.h ./deps/stream/stream.h ./deps/net/net.h ./lib/url.h ./lib/buffered_io.h ./lib/common.h ./lib/request.h ./deps/stream/stream.h ./lib/request.h

# dependencies for ./deps/closure/closure.c
./deps/closure/closure.o: ./deps/closure/closure.c

# dependencies for ./lib/buffered_io.c
./lib/buffered_io.o: ./lib/buffered_io.c ./deps/closure/closure.h ./deps/stream/stream.h ./deps/stream/stream.h ./deps/closure/closure.h

http: ./deps/stream/stream.o ./deps/stream/file.o ./lib/common.o ./lib/url.o ./deps/net/socket.o ./deps/net/tlssocket.o ./deps/net/net.o ./lib/request.o ./deps/closure/closure.o ./lib/buffered_io.o ./lib/response.o ./lib/http.o ./http.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS)  ./deps/stream/stream.o ./deps/stream/file.o ./lib/common.o ./lib/url.o ./deps/net/socket.o ./deps/net/tlssocket.o ./deps/net/net.o ./lib/request.o ./deps/closure/closure.o ./lib/buffered_io.o ./lib/response.o ./lib/http.o ./http.o -o http

CLEAN_http:
	rm -rf http  ./deps/stream/stream.o ./deps/stream/file.o ./lib/common.o ./lib/url.o ./deps/net/socket.o ./deps/net/tlssocket.o ./deps/net/net.o ./lib/request.o ./deps/closure/closure.o ./lib/buffered_io.o ./lib/response.o ./lib/http.o ./http.o
