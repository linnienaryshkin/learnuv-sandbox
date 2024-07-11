#include "learnuv.h"

#define BUF_SIZE 37
static const char *filename = __MAGIC_FILE__;

typedef struct context_struct
{
  uv_fs_t *open_req;
  uv_buf_t iov;
} context_t;

void read_cb(uv_fs_t *read_req)
{
  int r = 0;
  if (read_req->result < 0)
  {
    CHECK(read_req->result, "uv_fs_read callback");
  }

  /* extracting our context from the read_req */
  context_t *context = read_req->data;

  /* 4. Report the contents of the buffer */
  log_report("%s", context->iov.base);
  log_info("%s", context->iov.base);

  /**
   * TODO: Find out why freeing isn't working here.
   * 05_fs_readasync_context(65289,0x1ea364c00) malloc: *** error for object 0x16fdfef02: pointer being freed was not allocated
   */
  // free(context->iov.base);

  /* 5. Close the file descriptor (synchronously) */
  uv_fs_t close_req;
  r = uv_fs_close(read_req->loop, &close_req, context->open_req->result, NULL);
  if (r < 0)
  {
    CHECK(r, "uv_fs_close");
  }

  /* cleanup all requests and context */
  uv_fs_req_cleanup(context->open_req);
  uv_fs_req_cleanup(read_req);
  uv_fs_req_cleanup(&close_req);
  free(context);
}

void init(uv_loop_t *loop)
{
  int r = 0;

  /**
   * We need to malloc these requests because if we would use automatic variables instead, they would be
   * "automatically" deallocated once we leave the init function body.
   * However we need them to stay around since the read_cb will be invoked asynchronously.
   */
  uv_fs_t *open_req = malloc(sizeof(uv_fs_t));
  uv_fs_t *read_req = malloc(sizeof(uv_fs_t));

  context_t *context = malloc(sizeof(context_t));
  context->open_req = open_req;

  /* 1. Open file */
  r = uv_fs_open(loop, open_req, filename, O_RDONLY, S_IRUSR, NULL);
  if (r < 0)
  {
    CHECK(r, "uv_fs_open");
  }

  /* 2. Create buffer and initialize it to turn it into a a uv_buf_t */
  char buf[BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));
  uv_buf_t iov = uv_buf_init(buf, BUF_SIZE);
  context->iov = iov;

  /* allow us to access the context inside read_cb */
  read_req->data = context;

  /* 3. Read from the file into the buffer */
  //  r = uv_fs_read(loop, read_req, open_req, &iov, 1, 0, read_cb);
  r = uv_fs_read(loop, read_req, open_req->result, &iov, 1, 0, read_cb);
  if (r < 0)
  {
    CHECK(r, "uv_fs_read");
  }
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
  log_info("running 05_fs_readasync_context");

  uv_loop_t *loop = uv_default_loop();

  init(loop);

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
