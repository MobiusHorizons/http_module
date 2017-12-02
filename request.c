#include "../net/net.h"
#include "../stream/stream.h"

#include "./url.h"
#include "./common.h"

#include <string.h>
#include <stdio.h>

typedef struct {
  common_params_t  params;
  url_Url *        url;
  stream_t *       conn;
} request_request_t;

request_request_t request_request(common_params_t p) {
  char buf[4096];
  char * c = buf;
  int error;

  /* set defaults */
  p.method = (p.method == UNDEFINED_METHOD) ? GET : p.method;

  url_Url * u   = url_parse(p.url, &error);
  stream_t * conn = net_connect(u->host, u->port, u->is_tls);

  if (conn->error.code != 0) {
    request_request_t req = {
      .params = p,
      .conn   = conn,
      .url    = u,
    };

    return req;
  }

  /* write method and host lines */
  c += snprintf(c, 4096 - (c - buf), "%s /%s%s%s %s\r\n",
      common_method_names[p.method],
      u->route ? u->route : "",
      u->qs ? "?": "", u->qs ? u->qs : "",
      common_HTTP_VERSION
  );
  c += snprintf(c, 4096 - (c - buf), "Host: %s \r\n",
      u->host_port ? u->host_port: u->host
  );

  common_write(conn, buf, c - buf, p.verbose);

  /* write headers */
  char ** header = p.headers;
  while(header != NULL && *header != NULL){
    common_write(conn, *header, strlen(*header), p.verbose);
    common_write(conn, "\r\n", 2, p.verbose);
    header ++;
  }

  /* complete headers */
  common_write(conn, "\r\n", 2, p.verbose);

  /* return request */
  request_request_t req = {
    .params = p,
    .conn   = conn,
    .url    = u,
  };

  return req;
}
