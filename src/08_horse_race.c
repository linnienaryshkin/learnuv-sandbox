#define _BSD_SOURCE

#include "learnuv.h"
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define HORSE_WIDTH 29
#define HORSE_HEIGHT 11
#define TRACK_WIDTH 110 // 110
#define TRACKS 5        // 5
#define THREADS "5"     // 5

// http://tldp.org/HOWTO/NCURSES-Programming-HOWTO
// http://www.gnu.org/software/ncurses/ncurses.html
#define DRAW 0 // curses isn't working for me. Set to 0 to see log output.

typedef struct
{
  uv_async_t async;
  char *name;
  int color;
  int track;
  int speed;
  int position;
} horse_t;

const static char *horse_pic[HORSE_HEIGHT] = {
    "                 ,***,    ",
    "               ,***` *==--",
    "              ,**`( '|    ",
    "             ,**@ /v_/    ",
    "   ,*.-'''--*** '@@__     ",
    "  **/             |__`v   ",
    " .*'v     |   v   /  //   ",
    " ,*' >   .'----v |  [/    ",
    "    < <<`       ||        ",
    "     `vvv       ||        ",
    "       )vv      )v        "};

static horse_t horses[TRACKS] = {
    {.name = "bnoordhuis  ", .color = 1, .track = 0, .position = 0},
    {.name = "piscisaureus", .color = 2, .track = 1, .position = 0},
    {.name = "ry          ", .color = 3, .track = 2, .position = 0},
    {.name = "saghul      ", .color = 4, .track = 3, .position = 0},
    {.name = "indutny     ", .color = 5, .track = 4, .position = 0}};

int placement = 1;

static void load_color_palette()
{
  if (!has_colors())
  {
    return;
  }

  use_default_colors();
  start_color();
  init_pair(1, COLOR_BLUE, -1);
  init_pair(2, COLOR_GREEN, -1);
  init_pair(3, COLOR_CYAN, -1);
  init_pair(4, COLOR_RED, -1);
  init_pair(5, COLOR_YELLOW, -1);
}

static void init_screen()
{
  initscr();
  curs_set(FALSE);
  raw();
  noecho();
  refresh();
  load_color_palette();
}

void draw_horse(horse_t *horse)
{
  if (!DRAW)
  {
    log_info("Horse %s\ttrack: %d\t position: %d", horse->name, horse->track, horse->position);
    return;
  }

  int i;
  attrset(COLOR_PAIR(horse->color));
  for (i = 0; i < HORSE_HEIGHT; i++)
  {
    mvprintw((horse->track * HORSE_HEIGHT) + i, horse->position, horse_pic[i]);
  }
  refresh();
}

void progress_cb(uv_async_t *async)
{
  horse_t *horse = async->data;
  draw_horse(horse);
  return;
}

void race_cb(uv_work_t *work_req)
{
  int r = 0;
  horse_t *horse = work_req->data;
  double speed = 10;
  int i;

  for (i = 0; i < TRACK_WIDTH; i++)
  {
    horse->position++;

    /* 3. Send progress report so we can redraw the position of the horse every time its position changed */
    // http://docs.libuv.org/en/latest/async.html#c.uv_async_send
    r = uv_async_send(&horse->async);
    CHECK(r, "uv_async_send");

    speed += ((rand() % 5) - 2);

    /* whiplash in case our horse starts slowing down too much */
    if (speed < 5)
    {
      speed = 7;
    }

    usleep(1E6 / speed);
  }
}

void finished_race_cb(uv_work_t *work_req, int status)
{
  CHECK(status, "finished_race_cb");
  horse_t *horse = work_req->data;

  if (!DRAW)
  {
    log_info("Horse %s finished in place %d\ttrack: %d", horse->name, placement, horse->track);
  }
  log_report("Horse %s finished in place %d", horse->name, placement);

  placement++;

  // https://docs.libuv.org/en/latest/handle.html#c.uv_close
  uv_close((uv_handle_t *)&horse->async, NULL);
}

void add_horse(uv_loop_t *loop, int track)
{
  int r = 0;
  horse_t *horse = horses + track;
  // https://docs.libuv.org/en/latest/threadpool.html
  uv_work_t *work_req = malloc(sizeof(uv_work_t));

  work_req->data = horse;
  horse->async.data = horse;

  /* 1. Init async worker (attached to our horse) passing a callback to report progress */
  // http://docs.libuv.org/en/latest/async.html#c.uv_async_init
  r = uv_async_init(loop, &horse->async, progress_cb);
  CHECK(r, "uv_async_init");

  /* 2. Queue work for our worker passing the right callbacks */
  // http://docs.libuv.org/en/latest/threadpool.html#c.uv_queue_work
  r = uv_queue_work(loop, (uv_work_t *)work_req, race_cb, finished_race_cb);
  CHECK(r, "uv_async_init");

  if (!DRAW)
  {
    log_info("Queued horse %s on track: %d", horse->name, horse->track);
  }
}

int main(void)
{
  int i;

  /* Ensure that each horse gets its own thread, the default libuv threadpool size is 4 */
  setenv("UV_THREADPOOL_SIZE", THREADS, 1);

  srand(time(NULL));

  uv_loop_t *loop = uv_default_loop();

  if (DRAW)
  {
    init_screen();
  }

  for (i = 0; i < TRACKS; i++)
  {
    add_horse(loop, i);
  }

  uv_run(loop, UV_RUN_DEFAULT);

  if (DRAW)
  {
    endwin();
  }
  return 0;
}
