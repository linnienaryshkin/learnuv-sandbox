# experiment-libuv

This is a fork of [thlorenz/learnuv](https://github.com/thlorenz/learnuv), where I did my own exploration.

## How it started

It all started with this article series: [Event Loop and the Big Picture — NodeJS Event Loop Part 1](https://blog.insiderattack.net/event-loop-and-the-big-picture-nodejs-event-loop-part-1-1cb67a182810).

The articles explain how the Node.js event loop works under the hood, and how it maps to libuv phases. I wanted to go deeper — not just read about it, but actually run libuv locally and see it in action.

So I forked `learnuv` and worked through the C exercises to understand the raw event loop: timers, I/O polling, idle handlers, check handlers, and close handlers.

Once I had a feel for the libuv internals, I went back to Node.js and wrote [`nodejs/event_loop_sandbox/index.js`](nodejs/event_loop_sandbox/index.js) — a JavaScript sandbox that maps each libuv concept onto the corresponding Node.js APIs (`setTimeout`, `setImmediate`, `process.nextTick`, Promises, etc.).

## Repository structure

```
src/                        # C exercises using libuv (forked from learnuv)
  01_system_info.c
  02_idle.c
  03_fs_readsync.c
  04_fs_readasync.c
  05_fs_readasync_context.c
  06_fs_allasync.c
  07_tcp_echo_server.c
  08_horse_race.c
  event_loop_sandbox/       # libuv event loop diagrams
    libuv-event-loop.png
    node-event-loop.png
  ...
nodejs/
  event_loop_sandbox/
    index.js                # Node.js sandbox mapping libuv phases to JS APIs
```

## Before You Start (C side)

1. C Introduction — https://www.w3schools.com/c/c_intro.php
2. Pointers — https://www.w3schools.com/c/c_pointers.php
3. Address-Of / Dereference
4. Struct — https://www.w3schools.com/c/c_structs.php
5. Automatic Allocation vs Dynamic Allocation (malloc)
6. NULL
7. Typecast — https://www.tutorialspoint.com/cprogramming/c_type_casting.htm
8. LibUV guidebook (5 mins reading) — https://thlorenz.com/learnuv/book/

And the official [LibUV documentation](https://docs.libuv.org/en/v1.x/guide/introduction.html).

## Setup for `macOS 14.5 23F79`

```sh
npm install
```

```sh
pyenv install 2.7 &&
pyenv local 2.7.18
```

```sh
pip install six # https://github.com/thlorenz/learnuv/issues/45
```

```sh
eval "$(pyenv init -)" # for some reason it's not working from ~/.zshrc
```

```sh
./learnuv
```

```sh
./learnuv xcode
```

```sh
open learnuv.xcodeproj
```

```sh
./learnuv verify
```

## Running the Node.js sandbox

```sh
node nodejs/event_loop_sandbox
```

## License

MIT
