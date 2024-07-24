#include "learnuv.h"

static const char *filename = __MAGIC_FILE__;

void async_hello_world_cb(uv_work_t *work_req)
{
    log_info("Hello world");
    return;
}

void finish_cb(uv_work_t *work_req, int status)
{
    CHECK(status, "finish_cb");
    return;
}

int main()
{
    setenv("UV_THREADPOOL_SIZE", "0", 1);

    int r;
    // event-driven
    uv_loop_t *loop = uv_default_loop();

    // https://docs.libuv.org/en/latest/threadpool.html
    uv_work_t *work_req = malloc(sizeof(uv_work_t));

    r = uv_queue_work(loop, work_req, async_hello_world_cb, NULL);
    CHECK(r, "uv_async_init");

    uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}
