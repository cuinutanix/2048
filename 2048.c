/*
 * Copyright (c) 2014 Nutanix Inc. All rights reserved.
 *
 * Author: cui@nutanix.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "types.h"
#include "console.h"
#include "fb.h"
#include "image.h"

#include "2048.h"
#include "random.h"

#include "assets/tile_0.h"
#include "assets/tile_2.h"
#include "assets/tile_4.h"
#include "assets/tile_8.h"
#include "assets/tile_16.h"
#include "assets/tile_32.h"
#include "assets/tile_64.h"
#include "assets/tile_128.h"
#include "assets/tile_256.h"
#include "assets/tile_512.h"
#include "assets/tile_1024.h"
#include "assets/tile_2048.h"
#include "assets/tile_4096.h"
#include "assets/tile_8192.h"
#include "assets/tile_16384.h"
#include "assets/tile_32768.h"
#include "assets/tile_65536.h"
#include "assets/tile_131072.h"
#include "assets/container.h"

#include "assets/title.h"

static const u16 *const tiles[] = {
  tile_0_image,
  tile_2_image,
  tile_4_image,
  tile_8_image,
  tile_16_image,
  tile_32_image,
  tile_64_image,
  tile_128_image,
  tile_256_image,
  tile_512_image,
  tile_1024_image,
  tile_2048_image,
  tile_4096_image,
  tile_8192_image,
  tile_16384_image,
  tile_32768_image,
  tile_65536_image,
  tile_131072_image,
};

#define DIM 4

typedef struct {
  u8 board[DIM][DIM];
  u32 moves;
  u32 score;
} game_state_t;

static game_state_t active_game;
#define board active_game.board
#define moves active_game.moves
#define score active_game.score

static game_state_t saved_games[10];

static int moved = 0;

#define TILE_WIDTH   (tile_0_image[0])
#define TILE_HEIGHT  (tile_0_image[1])
#define OUTER_MARGIN 12
#define INNER_MARGIN 11
#define BOARD_WIDTH  (2*OUTER_MARGIN + (DIM-1)*INNER_MARGIN + TILE_WIDTH*DIM)
#define BOARD_HEIGHT (2*OUTER_MARGIN + (DIM-1)*INNER_MARGIN + TILE_HEIGHT*DIM)

#define BOARD_OFFSET_X 30
#define BOARD_OFFSET_Y 60

#define TILE_OFFSET_X(x)                        \
  (BOARD_OFFSET_X + OUTER_MARGIN + (INNER_MARGIN + TILE_WIDTH)  * (x))
#define TILE_OFFSET_Y(y)                        \
  (BOARD_OFFSET_Y + OUTER_MARGIN + (INNER_MARGIN + TILE_HEIGHT) * (y))

static void printnumber(int n)
{
  int modulo;

  if (n == 0) {
    putc('0');
    return;
  }

  for (modulo = (int)1e9; modulo > 0; modulo /= 10) {
    if (modulo > n) {
      continue;
    }
    putc('0' + (n / modulo) % 10);
  }
}

static void print_2048()
{
  int x, y;

  for (y = 0; y < DIM; y++) {
    for (x = 0; x < DIM; x++) {
      display_image(TILE_OFFSET_Y(y), TILE_OFFSET_X(x), tiles[board[y][x]]);
    }
  }

  set_cursor(16, 55);
  set_color(0x0f);
  print("Moves: "); printnumber(moves); putc('\n');

  set_cursor(18, 55);
  set_color(0x0f);
  print("Score: "); printnumber(score); putc('\n');
}

void game_reset()
{
  int x, y;
  u32 r = randint(DIM * DIM);

  display_image(BOARD_OFFSET_Y, BOARD_OFFSET_X, container_image);
  display_image(BOARD_OFFSET_Y + 60,
                BOARD_OFFSET_X + BOARD_WIDTH + 30, title_image);

  for (y = 0; y < DIM; y++) {
    for (x = 0; x < DIM; x++) {
      board[y][x] = 0;
    }
  }

  y = (r / DIM);
  x = r % DIM;
  board[y][x] = 1;
  moves = 0;
  score = 0;
  moved = 1;
}


static void transpose()
{
  int x, y;
  for (x = 0; x < DIM; x++) {
    for (y = 0; y < x; y++) {
      int tmp = board[x][y];
      board[x][y] = board[y][x];
      board[y][x] = tmp;
    }
  }
}

static void mirror()
{
  int x, y;
  for (y = 0; y < DIM; y++) {
    for (x = 0; x < DIM/2; x++) {
      int tmp = board[y][x];
      board[y][x] = board[y][DIM - 1 - x];
      board[y][DIM - 1 - x] = tmp;
    }
  }
}

static void add_cell()
{
  u8 *cells[DIM * DIM];
  int num_cells = 0;
  int x, y;

  for (y = 0; y < DIM; y++) {
    for (x = 0; x < DIM; x++) {
      u8 *const p = &board[y][x];
      if (*p == 0) {
        cells[num_cells++] = p;
      }
    }
  }

  if (num_cells > 0) {
    u32 r = randint(num_cells * 10);
    y = (r / 10) % num_cells;
    *cells[y] = (r % 10 == 0) ? 2 : 1;
  }
}

static int collect_row(u8 row[DIM])
{
  int i, j;
  int r = 0;

  for (i = 0; i < DIM; i++) {
    if (row[i] != 0) {
      continue;
    }
    for (j = i + 1; j < DIM; j++) {
      if (row[j] != 0) {
        row[i] = row[j];
        row[j] = 0;
        r = 1;
        break;
      }
    }
  }
  return r;
}

static int merge_row(u8 row[DIM])
{
  int i;
  int r = 0;

  for (i = 0; i < DIM - 1; i++) {
    if (row[i] != 0 && row[i] == row[i+1]) {
      row[i]++;
      row[i+1] = 0;
      r = 1;
      score += (1 << row[i]);
    }
  }

  return r;
}

static void merge_left()
{
  int y;
  int moved = 0;

  for (y = 0; y < DIM; y++) {
    moved += collect_row(board[y]);
    moved += merge_row(board[y]);
    moved += collect_row(board[y]);
  }

  if (moved) {
    moves += 1;
    add_cell();
  }
}

static void merge_right()
{
  mirror();
  merge_left();
  mirror();
}

static void merge_up()
{
  transpose();
  merge_left();
  transpose();
}

static void merge_down()
{
  transpose();
  merge_right();
  transpose();
}

void game_input(int key)
{
  if (moved) {
    return;
  }

  if (key == LEFT) {
    merge_left();
  } else if (key == UP) {
    merge_up();
  } else if (key == RIGHT) {
    merge_right();
  } else if (key == DOWN) {
    merge_down();
  }

  moved = 1;
}

#undef board
#undef moves
#undef score

void game_save(int session)
{
  saved_games[session] = active_game;
}

void game_restore(int session)
{
  if (saved_games[session].moves > 0) {
    active_game = saved_games[session];
    moved = 1;
  }
}

void game_tick()
{
  if (moved) {
    print_2048();
    moved = 0;
  }
}
