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

    // TODO: Call a timer, before handlers

    // https://docs.libuv.org/en/latest/threadpool.html
    uv_work_t *work_req = malloc(sizeof(uv_work_t));

    // TODO: Differentiate IDLE (is it uv_idle_t?) and PREPARE handlers |
    // TODO: Find out that I/O (try file reading, or networking (it's even better because we could initiate event via terminal)) is called before CLOSE handler
    // TODO: Understand once CLOSE handler is called
    r = uv_queue_work(loop, work_req, async_hello_world_cb, NULL);

    // TODO: Identify CHECK handlers
    CHECK(r, "uv_async_init");

    // TODO: Find where loop time is updated
    uv_run(loop, UV_RUN_DEFAULT);

    // TODO: But. Learn how multiple threads are actually working together inside one event loop

    // TODO: Just read about processes inside libuv, just to be aware

    // TODO: Try stopping event loop (and see how events are still processed before loop is stopped)

    return 0;
}
