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

// int64_t counter = 0;
// Despite the unfortunate name, idle handles are run on every loop iteration, if they are active.
void wait_for_a_while(uv_idle_t *handle)
{
    handle->data++;
    log_info("IDLE handler: %ld", handle->data);

    if (handle->data >= 100)
    {
        uv_idle_stop(handle);
    }
}

int main()
{
    setenv("UV_THREADPOOL_SIZE", "0", 1);

    int r;
    // TODO: Call a timer, before handlers

    // https://docs.libuv.org/en/latest/threadpool.html
    uv_work_t *work_req = malloc(sizeof(uv_work_t));

    uv_idle_t idler;
    idler.data = 0;
    uv_idle_init(uv_default_loop(), &idler);
    uv_idle_start(&idler, wait_for_a_while);

    // TODO: Differentiate IDLE (is it uv_idle_t?) and PREPARE handlers |
    // TODO: Find out that I/O (try file reading, or networking (it's even better because we could initiate event via terminal)) is called before CLOSE handler

    // uv_fs_t *open_req = malloc(sizeof(uv_fs_t));
    // r = uv_fs_open(uv_default_loop(), open_req, filename, O_RDONLY, S_IRUSR, NULL);
    // CHECK(r, "uv_fs_open");
    // char buf[20 + 1];
    // memset(buf, 0, sizeof(buf));
    // uv_buf_t iov = uv_buf_init(buf, 20);
    // uv_fs_t read_req;
    // r = uv_fs_read(uv_default_loop(), &read_req, open_req->result, &iov, 1, 0, NULL);

    // TODO: Understand once CLOSE handler is called
    r = uv_queue_work(uv_default_loop(), work_req, async_hello_world_cb, NULL);

    // TODO: Identify CHECK handlers
    CHECK(r, "uv_async_init");

    // TODO: Find where loop time is updated
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    // TODO: But. Learn how multiple threads are actually working together inside one event loop

    // TODO: Just read about processes inside libuv, just to be aware

    // TODO: Try stopping event loop (and see how events are still processed before loop is stopped)

    return 0;
}
