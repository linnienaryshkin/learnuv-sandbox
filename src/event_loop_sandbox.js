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

node src/event_loop_sandbox.js

 */

console.log("Start");

/**
 * demultiplexer
 * - receives I/O requests and delegates these requests to the appropriate hardware.
 * - add the registered callback handler for the particular action (events) in a queue (Event Queue) to be processed
 * - Reactor Pattern
 * - epoll on Linux, kqueue on BSD systems (macOS), event ports in Solaris, IOCP (Input Output Completion Port) in Windows, etc
 * - thread pool is used to handle the I/O operations, in case it cannot be directly addressed by hardware asynchronous I/O utils
 */

/**
 * Process vs Threads
 * The primary difference between a process and a thread is that
 * different processes cannot share the same memory space (code, variables, etc)
 * whereas different threads in the same process share the same memory space.
 * Threads are lightweight whereas Processes are heavyweight.
 */
