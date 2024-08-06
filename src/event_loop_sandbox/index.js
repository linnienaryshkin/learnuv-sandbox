/**
 * @fileoverview Event loop sandbox, try to understand how libuv works from POINT OF VIEW of JavaScript.
 * Inspired by this article circle
 * @link Event Loop and the Big Picture (This article) - https://blog.insiderattack.net/event-loop-and-the-big-picture-nodejs-event-loop-part-1-1cb67a182810
 * @link Timers, Immediates and Next Ticks - https://blog.insiderattack.net/timers-immediates-and-process-nexttick-nodejs-event-loop-part-2-2c53fd511bb3
 * @link Promises, Next-Ticks, and Immediates - https://blog.insiderattack.net/promises-next-ticks-and-immediates-nodejs-event-loop-part-3-9226cbe7a6aa
 * @link Handling I/O - https://blog.insiderattack.net/handling-io-nodejs-event-loop-part-4-418062f917d1
 * @link Event Loop Best Practices - https://blog.insiderattack.net/event-loop-best-practices-nodejs-event-loop-part-5-e29b2b50bfe2
 * @link New changes to timers and microtasks in Node v11 - https://blog.insiderattack.net/new-changes-to-timers-and-microtasks-from-node-v11-0-0-and-above-68d112743eb3
 * @link JavaScript Event Loop vs Node JS Event Loop - https://blog.insiderattack.net/javascript-event-loop-vs-node-js-event-loop-aea2b1b85f5c

node src/event_loop_sandbox

 */

console.log("Start of Script");

/**
 * demultiplexer (libuv, libevent for Chrome)
 * - receives I/O requests and delegates these requests to the appropriate hardware.
 * - add the registered callback handler for the particular action (events) in a queue (Event Queue) to be processed
 * - Reactor Pattern
 * - epoll on Linux, kqueue on BSD systems (macOS), event ports in Solaris, IOCP (Input Output Completion Port) in Windows, etc
 * - thread pool is used to handle the I/O operations, in case it cannot be directly addressed by hardware asynchronous I/O utils
 * - NodeJS has done its best to do most of the I/O using non-blocking and asynchronous hardware I/O, but for the I/O types which blocks or are complex to address, it uses the thread pool.
 */

/**
 * Process vs Threads
 * The primary difference between a process and a thread is that
 * different processes cannot share the same memory space (code, variables, etc)
 * whereas different threads in the same process share the same memory space.
 * Threads are lightweight whereas Processes are heavyweight.
 */

/**
 * Threadpool
 * - used to perform all File I/O operations, getaddrinfo and getnameinfo calls during DNS operations, randomBytes, randomFill and pbkdf2 are also run on the libuv thread pool
 * - thread pool is limited (default size is 4, can be increased up to 128)
 */

/**
 * Event Loop (libuv, libevent for Chrome)
 * - After processing one phase and before moving to the next phase, event loop will process two intermediate queues until no items are remaining in the intermediate queues.
 * - Tracking the reference counter of total items to be processed - once it reaches zero, the event loop exits.
 * 1. Expired timers and intervals queue (min-heap) (setTimeout, setInterval)
 * 2. IO Events Queue
 * 3. Immediate Queue (setImmediate)
 * 4. Close Handlers Queue
 * 0. (before moving between queues) Next Ticks Queue, Other Microtasks Queue (resolved/reject promise callbacks)
 * - Next Ticks and Microtasks Queue will run between each individual setTimeout and setImmediate callbacks to match the browser behavior
 */

/**
 * Libuv Event Loop Phases
 * 1. Timers — Expired timer and interval callbacks scheduled by `setTimeout` and `setInterval` will be invoked.
 * 2. Pending I/O callbacks — Pending Callbacks of any completed/errored I/O operation to be executed here.
 * 3. Idle, prepare — Used internally by NodeJS.
 * 4. Prepare Handlers — Perform some prep-work before polling for I/O.
 * 5. I/O Poll — Optionally wait for any I/O to complete.
 * 6. Check handlers — Perform some post-mortem work after polling for I/O. Usually, callbacks scheduled by setImmediate will be invoked here.
 * 7. Close handlers — Execute close handlers of any closed I/O operations (closed socket connection etc.)
 * 
 * @link https://www.youtube.com/watch?v=sGTRmPiXD4Y

// Check whether there are any referenced handlers to be invoked, or any active operations pending
r = uv__loop_alive(loop);
if (!r)
  uv__update_time(loop);

while (r != 0 && loop->stop_flag == 0) {
  // This will send a system call to get the current time and update the loop time (This is used to identify expired timers).
  uv__update_time(loop);

  // Run all expired timers
  uv__run_timers(loop);
  
  // Run all completed/errored I/O callbacks
  ran_pending = uv__run_pending(loop);
  
  uv__run_idle(loop);
  uv__run_prepare(loop);

  // timeout to determine how long it should block for I/O
  timeout = 0;
  if ((mode == UV_RUN_ONCE && !ran_pending) || mode == UV_RUN_DEFAULT)
    timeout = uv_backend_timeout(loop);

  // Poll for I/O, epoll_wait on Linux, kqueue on BSD systems (macOS), event ports in Solaris, GetQueuedCompletionStatus in IOCP (Input Output Completion Port) in Windows, etc
  uv__io_poll(loop, timeout);

  // Run all check handlers (setImmediate callbacks will run here)
  uv__run_check(loop);

  // Run all close handlers
  uv__run_closing_handles(loop);

  if (mode == UV_RUN_ONCE) {
    uv__update_time(loop);
    uv__run_timers(loop);
  }

  r = uv__loop_alive(loop);
  if (mode == UV_RUN_ONCE || mode == UV_RUN_NOWAIT)
    break;
}


// Event loop will keep spinning as long as uv__loop_alive function returns true.
static int uv__loop_alive(const uv_loop_t* loop) {
  return uv__has_active_handles(loop) ||
    uv__has_active_reqs(loop) ||
    loop->closing_handles != NULL;
}

static int uv__run_pending(uv_loop_t* loop) {
  QUEUE* q;
  QUEUE pq;
  uv__io_t* w;

  if (QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  QUEUE_MOVE(&loop->pending_queue, &pq);

  while (!QUEUE_EMPTY(&pq)) {
    q = QUEUE_HEAD(&pq);
    QUEUE_REMOVE(q);
    QUEUE_INIT(q);
    w = QUEUE_DATA(q, uv__io_t, pending_queue);
    w->cb(loop, w, POLLOUT);
  }

  return 1;
}

int uv_backend_timeout(const uv_loop_t* loop) {
  if (loop->stop_flag != 0)
    return 0;

  if (!uv__has_active_handles(loop) && !uv__has_active_reqs(loop))
    return 0;

  if (!QUEUE_EMPTY(&loop->idle_handles))
    return 0;

  if (!QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  if (loop->closing_handles)
    return 0;

  return uv__next_timeout(loop);
}

// The event loop will not be blocked if there are any pending tasks to be executed. If there are no pending tasks to be executed, it will only be blocked until the next timer goes off, which re-activates the loop.
int uv__next_timeout(const uv_loop_t* loop) {
  const struct heap_node* heap_node;
  const uv_timer_t* handle;
  uint64_t diff;

  heap_node = heap_min((const struct heap*) &loop->timer_heap);
  if (heap_node == NULL)
    return -1; // block indefinitely

  handle = container_of(heap_node, uv_timer_t, heap_node);
  if (handle->timeout <= loop->time)
    return 0;

  diff = handle->timeout - loop->time;
  if (diff > INT_MAX)
    diff = INT_MAX;

  return diff;
}
 */

/**
 * Next tick queue is not natively provided by the libuv (four main phases), but implemented in Node.
 */
function nextTickQueue() {
  setTimeout(() => console.log("timeout1"));
  setTimeout(() => {
    console.log("timeout2");
    Promise.resolve().then(() => console.log("promise resolve"));
  });
  setTimeout(() => console.log("timeout3"));
  setTimeout(() => console.log("timeout4"));
}
// nextTickQueue();

/**
 * IO starvation
 * - Extensively filling up the next tick queue using process.nextTick function will force the event loop to keep processing the next tick queue indefinitely without moving forward.
 * TODO: Write a code snippet to demonstrate IO starvation
 */
function ioStarvation() {
  function addNextTickRecurs(count) {
    let self = this;
    if (self.id === undefined) {
      self.id = 0;
    }

    if (self.id === count) return;

    process.nextTick(() => {
      console.log(`process.nextTick call ${++self.id}`);
      addNextTickRecurs.call(self, count);
    });
  }

  addNextTickRecurs(10);
  // Node add a timer to the timers heap (in memory)
  setTimeout(console.log.bind(console, "omg! setTimeout was called"), 10);
  setImmediate(console.log.bind(console, "omg! setImmediate also was called"));
}
// ioStarvation();

/**
 * Node check against time expiration.
 * Time is taken on each loop run
 * @link https://docs.libuv.org/en/v1.x/design.html#the-i-o-loop | concept of ‘now’
 */
function TimerExecution() {
  const start = process.hrtime();

  setTimeout(() => {
    const end = process.hrtime(start);
    console.log(
      `timeout callback executed after ${end[0]}s and ${
        end[1] / Math.pow(10, 9)
      }ms`
    );
  }, 1000);
}
// TimerExecution();

/**
 * As you might guess, this program will always print setTimeout before setImmediate
 * because the expired timer queue are processed before immediate.
 * !BUT
 * The minimum timeout to 1ms in order to align with Chrome’s timers cap
 * @link https://chromium.googlesource.com/chromium/blink/+/master/Source/core/frame/DOMTimer.cpp#93
 * So sometimes, setImmediate will be executed before setTimeout.
 */
function TimeoutVsImmediate() {
  setTimeout(function () {
    console.log("setTimeout");
  }, 0);
  setImmediate(function () {
    console.log("setImmediate");
  });
}
// TimeoutVsImmediate();

/**
 * Next Ticks and Microtasks (resolved/reject promise) Queues (0) are processed before every phase of the event loop.
 */
function PromiseVsImmediate() {
  new Promise(() => console.log("promise executor"));
  Promise.resolve().then(() => console.log("promise1 resolved"));
  Promise.resolve().then(() => console.log("promise2 resolved"));
  Promise.resolve().then(() => {
    console.log("promise3 resolved");
    process.nextTick(() =>
      console.log("next tick inside promise resolve handler")
    );
  });
  Promise.resolve().then(() => console.log("promise4 resolved"));
  Promise.resolve().then(() => console.log("promise5 resolved"));
  /**
   * Timeout (1) handled earlier than Immediate (3)
   */
  setTimeout(() => console.log("setTimeout"), 0);
  setImmediate(() => console.log("set immediate1"));
  setImmediate(() => console.log("set immediate2"));
}
// PromiseVsImmediate();

/**
 * It's the same example as PromiseVsImmediate
 */
function MicrotasksVsMacrotasks() {
  queueMicrotask(() => console.log("queueMicrotask1 resolved"));
  queueMicrotask(() => console.log("queueMicrotask2 resolved"));
  setTimeout(() => {
    console.log("set timeout3");
    queueMicrotask(() => console.log("inner queueMicrotask3 resolved"));
  }, 0);
  setTimeout(() => console.log("set timeout1"), 0);
  setTimeout(() => console.log("set timeout2"), 0);
  queueMicrotask(() => console.log("queueMicrotask4 resolved"));
  queueMicrotask(() => {
    console.log("queueMicrotask5 resolved");
    queueMicrotask(() => console.log("inner queueMicrotask6 resolved"));
  });
  queueMicrotask(() => console.log("queueMicrotask7 resolved"));
}
// MicrotasksVsMacrotasks();

/**
 * Even if you have set the timeout to 0, all NodeJS timers seem to have fired after at least1ms.
 * Chrome seems to cap the minimum timeout to 1ms for the first 4 nested timers. But afterwards, the cap seems to be increased to 4ms.
 * > “Timers can be nested; after five such nested timers, however, the interval is forced to be at least four milliseconds.”
 * Both NodeJS and Chrome enforces a 1ms minimum timeout to all the timers
 */
function TimerRace() {
  const startHrTime = () => {
    if (typeof window !== "undefined") return performance.now();
    return process.hrtime();
  };

  const getHrTimeDiff = (start) => {
    if (typeof window !== "undefined") return performance.now() - start;
    const [ts, tns] = process.hrtime(start);
    return ts * 1e3 + tns / 1e6;
  };

  console.log("TimerRace starts");
  const start1 = startHrTime();
  const outerTimer = setTimeout(() => {
    const start2 = startHrTime();
    console.log(`timer1: ${getHrTimeDiff(start1)}`);
    setTimeout(() => {
      const start3 = startHrTime();
      console.log(`timer2: ${getHrTimeDiff(start2)}`);
      setTimeout(() => {
        const start4 = startHrTime();
        console.log(`timer3: ${getHrTimeDiff(start3)}`);
        setTimeout(() => {
          const start5 = startHrTime();
          console.log(`timer4: ${getHrTimeDiff(start4)}`);
          setTimeout(() => {
            const start6 = startHrTime();
            console.log(`timer5: ${getHrTimeDiff(start5)}`);
            setTimeout(() => {
              const start7 = startHrTime();
              console.log(`timer6: ${getHrTimeDiff(start6)}`);
              setTimeout(() => {
                const start8 = startHrTime();
                console.log(`timer7: ${getHrTimeDiff(start7)}`);
                setTimeout(() => {
                  console.log(`timer8: ${getHrTimeDiff(start8)}`);
                });
              });
            });
          });
        });
      });
    });
  });
}
TimerRace();

console.log("End of Script");
