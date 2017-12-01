# Variables
PREFIX ?= /usr/local
CFLAGS += -Wno-nullability-completeness

.PHONY: clean all dist install

all : http

http.mk: http.module.c
	cbuild http.module.c

clean :
	cbuild clean http.module.c

dist: http CLEAN_http

install: http
	cp http $(PREFIX)/bin/http

include http.mk
