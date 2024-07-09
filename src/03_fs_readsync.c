#include "learnuv.h"

#define BUF_SIZE 37
static const char *filename = __MAGIC_FILE__;

int main()
{
  log_info("running 03_fs_readsync");

  int r = 0;
  uv_loop_t *loop = uv_default_loop();

  /**
   * Main Resources
   * [official libuv documentation](http://docs.libuv.org/en/latest/index.html) *you should make use of the "Quick Search" box*
   * [Unoffical libuv dox examples](https://github.com/thlorenz/libuv-dox/tree/master/examples)
   *
   * Resources
   * [`uv_fs_open`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_open)
   * [`uv_buf_init`](http://docs.libuv.org/en/latest/misc.html#c.uv_buf_init)
   * [`uv_fs_read`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_read)
   * [`uv_fs_close`](http://docs.libuv.org/en/latest/fs.html#c.uv_fs_close)
   */

  /**
   * 1. Open file
   * When opening a file pass O_RDONLY as the flags and S_IRUSR as the mode.
   * All uv_fs_* methods pass back a signed integer that needs to be checked. If it is negative an error occurred.
   * These checks have been added for you already, but pay close attention as in the future you will need to add them yourself.
   */
  uv_fs_t open_req;
  r = uv_fs_open(loop, &open_req, filename, O_RDONLY, S_IRUSR, NULL);

  if (r < 0)
  {
    CHECK(r, "uv_fs_open");
  }

  /* 2. Create buffer and initialize it to turn it into a a uv_buf_t which adds length field */
  /* The + 1 is for the string null terminator, because log_report and log_info need it */
  /* Make sure you set your uv_buf_t size to BUF_SIZE instead of sizeof(buf) */
  char buf[BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));
  uv_buf_t iov = uv_buf_init(buf, BUF_SIZE);

  /* 3. Use the file descriptor (the .result of the open_req) to read from the file into the buffer */
  uv_fs_t read_req;
  r = uv_fs_read(loop, &read_req, open_req.result, &iov, 1, 0, NULL);

  if (r < 0)
  {
    CHECK(r, "uv_fs_read");
  }

  /* 4. Report the contents of the buffer */
  log_report("%s", buf);

  log_info("%s", buf);

  /* 5. Close the file descriptor (`open_req.result`) */
  uv_fs_t close_req;
  r = uv_fs_close(loop, &close_req, open_req.result, NULL);

  if (r < 0)
  {
    CHECK(r, "uv_fs_close");
  }

  /* always clean up your requests when you no longer need them to free unused memory */
  uv_fs_req_cleanup(&open_req);
  uv_fs_req_cleanup(&read_req);
  uv_fs_req_cleanup(&close_req);

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
