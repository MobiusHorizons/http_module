import url      from "./url.module.c";
import net      from "../deps/net/net.module.c";
import stream   from "../deps/stream/stream.module.c";
import common   from "./common.module.c";

#include <string.h>
#include <stdio.h>

export typedef struct {
  common.params_t  params;
  url.Url *        url;
  stream.t *       conn;
} request_t;

export request_t request(common.params_t p) {
  char buf[4096];
  char * c = buf;
  int error;

  /* set defaults */
  p.method = (p.method == UNDEFINED_METHOD) ? GET : p.method;

  url.Url * u   = url.parse(p.url, &error);
  stream.t * conn = net.connect(u->host, u->port, u->is_tls);

  if (conn->error.code != 0) {
    request_t req = {
      .params = p,
      .conn   = conn,
      .url    = u,
    };

    return req;
  }

  /* write method and host lines */
  c += snprintf(c, 4096 - (c - buf), "%s /%s%s%s %s\r\n",
      common.method_names[p.method],
      u->route ? u->route : "",
      u->qs ? "?": "", u->qs ? u->qs : "",
      common.HTTP_VERSION
  );
  c += snprintf(c, 4096 - (c - buf), "Host: %s \r\n",
      u->host_port ? u->host_port: u->host
  );

  common.write(conn, buf, c - buf, p.verbose);

  /* write headers */
  char ** header = p.headers;
  while(header != NULL && *header != NULL){
    common.write(conn, *header, strlen(*header), p.verbose);
    common.write(conn, "\r\n", 2, p.verbose);
    header ++;
  }

  /* complete headers */
  common.write(conn, "\r\n", 2, p.verbose);

  /* return request */
  request_t req = {
    .params = p,
    .conn   = conn,
    .url    = u,
  };

  return req;
}
