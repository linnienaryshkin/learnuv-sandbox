#include "learnuv.h"

#define BUF_SIZE 37
static const char *filename = __MAGIC_FILE__;

/**
 * [`uv_fs_open`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_open)
 * [`uv_buf_init`](http://docs.libuv.org/en/latest/misc.html#c.uv_buf_init)
 * [`uv_fs_read`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_read)
 * [`uv_fs_close`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_close)
 * [`uv_read_cb`](http://docs.libuv.org/en/latest/stream.html#c.uv_read_cb)
 * [`uv_fs_req_cleanup`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_req_cleanup)
 */

/* forward declarations */
void open_cb(uv_fs_t *);
void read_cb(uv_fs_t *);
void close_cb(uv_fs_t *);

typedef struct context_struct
{
  uv_fs_t *open_req;
  uv_fs_t *read_req;
  uv_buf_t iov;
} context_t;

void open_cb(uv_fs_t *open_req)
{
  int r = 0;
  if (open_req->result < 0)
  {
    CHECK(open_req->result, "uv_fs_open callback");
  }

  context_t *context = open_req->data;

  /* 3. Create buffer and initialize it */
  char buf[BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));
  uv_buf_t iov = uv_buf_init(buf, BUF_SIZE);
  context->iov = iov;

  /* 4. Setup read request */
  uv_fs_t *read_req = malloc(sizeof(uv_fs_t));
  context->read_req = read_req;
  read_req->data = context;

  /* 5. Read from the file into the buffer */
  r = uv_fs_read(open_req->loop, read_req, open_req->result, &iov, 1, 0, read_cb);
  if (r < 0)
  {
    CHECK(r, "uv_fs_read");
  }
}

void read_cb(uv_fs_t *read_req)
{
  int r = 0;
  if (read_req->result < 0)
  {
    CHECK(read_req->result, "uv_fs_read callback");
  }

  context_t *context = read_req->data;

  /* 6. Report the contents of the buffer */
  log_report("%s", context->iov.base);
  log_info("%s", context->iov.base);

  // free(context->iov.base);

  /* 7. Setup close request */
  // uv_fs_t close_req;
  // close_req.data = context;
  uv_fs_t *close_req = malloc(sizeof(uv_fs_t));
  close_req->data = context;

  /* 8. Close the file descriptor */
  r = uv_fs_close(read_req->loop, close_req, context->open_req->result, close_cb);
  if (r < 0)
  {
    CHECK(r, "uv_fs_close");
  }
}

void close_cb(uv_fs_t *close_req)
{
  if (close_req->result < 0)
  {
    CHECK(close_req->result, "uv_fs_close callback");
  }

  context_t *context = close_req->data;

  /* 9. Cleanup all requests and context */
  uv_fs_req_cleanup(context->open_req);
  uv_fs_req_cleanup(context->read_req);
  uv_fs_req_cleanup(close_req);
  free(context);
}

void init(uv_loop_t *loop)
{
  int r;
  uv_fs_t *open_req = malloc(sizeof(uv_fs_t));

  /* 1. Setup open request */
  context_t *context = malloc(sizeof(context_t));
  context->open_req = open_req;
  open_req->data = context;

  /* 2. Open file */
  r = uv_fs_open(loop, open_req, filename, O_RDONLY, S_IRUSR, open_cb);
  if (r < 0)
  {
    CHECK(r, "uv_fs_open");
  }
}

int main()
{
  log_info("running 06_fs_allasync");

  uv_loop_t *loop = uv_default_loop();

  init(loop);

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
