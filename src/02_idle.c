#include "learnuv.h"

void idle_cb(uv_idle_t *handle)
{
  static int64_t count = -1;
  count++;
  if ((count % 10000) == 0)
  {
    log_info(".");
    log_report(".");
  }
  if (count >= 50000)
  {
    log_info("uv_idle_stop");
    uv_idle_stop(handle);
  }
}

int main()
{
  log_info("running 02_idle");

  uv_idle_t idle;

  /* 1. create the event loop */
  // TODO: Why * here?
  // http://docs.libuv.org/en/latest/loop.html
  uv_loop_t *loop = uv_default_loop();

  /* 2. init an idle handler for the loop */
  // http://docs.libuv.org/en/latest/idle.html
  uv_idle_init(loop, &idle);

  /* 3. start the idle handler with a function to call */
  uv_idle_start(&idle, idle_cb);

  /* 4. start the event loop */
  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
