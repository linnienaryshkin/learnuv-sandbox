#include "learnuv.h"

#define BUF_SIZE 37
static const char *filename = __MAGIC_FILE__;

/* Making our lifes a bit easier by using this global, a better solution in the next exercise ;) */
static uv_fs_t open_req;
static uv_buf_t iov;

void read_cb(uv_fs_t *read_req)
{
  int r = 0;
  if (read_req->result < 0)
  {
    CHECK(read_req->result, "uv_fs_read callback");
  }

  /* 4. Report the contents of the buffer */
  log_report("%s", iov.base);
  log_info("%s", iov.base);

  /* 5. Close the file descriptor */
  uv_fs_t close_req;
  r = uv_fs_close(read_req->loop, &close_req, open_req.result, NULL);
  if (r < 0)
  {
    CHECK(r, "uv_fs_close");
  }

  uv_fs_req_cleanup(&open_req);
  uv_fs_req_cleanup(read_req);
  uv_fs_req_cleanup(&close_req);
}

/**
 * [`uv_fs_open`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_open)
 * [`uv_buf_init`](http://docs.libuv.org/en/latest/misc.html#c.uv_buf_init)
 * [`uv_fs_read`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_read)
 * [`uv_fs_close`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_close)
 * [`uv_read_cb`](http://docs.libuv.org/en/latest/stream.html#c.uv_read_cb)
 * [`uv_fs_req_cleanup`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_req_cleanup)
 */
int main()
{
  log_info("running 04_fs_readasync");

  int r = 0;
  uv_loop_t *loop = uv_default_loop();

  /* 1. Open file (synchronously) */
  r = uv_fs_open(loop, &open_req, filename, O_RDONLY, S_IRUSR, NULL);
  if (r < 0)
  {
    CHECK(r, "uv_fs_open");
  }

  /* 2. Create buffer and initialize it to turn it into a a uv_buf_t */
  char buf[BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));
  iov = uv_buf_init(buf, BUF_SIZE);

  /* 3. Use the file descriptor (the .result of the open_req) to read **aynchronously** from the file into the buffer */
  uv_fs_t read_req;
  r = uv_fs_read(loop, &read_req, open_req.result, &iov, 1, 0, read_cb);
  if (r < 0)
  {
    CHECK(r, "uv_fs_read");
  }

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
