#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BACKGROUND_COLOR 0x00
#define FOREGROUND_COLOR 0xe2
#define PAD_COLOR 0xff
#define WINDOW_WIDTH 100
#define WINDOW_HEIGHT 95
#define PADDING_SIZE 1

char fbuf[WINDOW_HEIGHT][WINDOW_WIDTH];

static void draw_background() {
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
    for (int j = 0; j < WINDOW_WIDTH; j++) {
      fbuf[i][j] = BACKGROUND_COLOR;
    }
  }
  return;
}

static void draw_padding() {
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
    for (int j = 0; j < WINDOW_WIDTH; j++) {
      if (i < PADDING_SIZE || i + PADDING_SIZE >= WINDOW_HEIGHT ||
          j < PADDING_SIZE || j + PADDING_SIZE >= WINDOW_WIDTH) {
          fbuf[i][j] = PAD_COLOR;
      }
    }
  }
  return;
}

#define RES (1 << 28)

void draw_brot(long long xmin, long long xmax, long long ymin, long long ymax) {
  for (int y = 0; y < WINDOW_HEIGHT; y++) {
    for (int x = 0; x < WINDOW_WIDTH; x++) {
      long long cr = (((long long)x) * (xmax - xmin))/WINDOW_WIDTH + xmin;
      long long ci = (((long long)y) * (ymax - ymin))/WINDOW_HEIGHT + ymin;
      long long r = 0, i = 0, temp;
      int iter = 0;
      while (((r * r + i * i) < 4ll*RES*RES) && (iter++ < 30)) {
        temp = (r * r - i * i)/RES + cr;
        i = (2 * r * i)/RES + ci;
        r = temp;
      }
      if (iter == 31) {
        fbuf[y][x] = 0;
      } else {
        fbuf[y][x] = (int)((0xff * iter)/30);
      }
    }
  }
}

void main(void) {
  draw_background();
  long long xmax = (1*RES)/2, ymax = (5*RES)/4, xmin = -2*RES, ymin = -(5*RES)/4;
  draw_brot(xmin, xmax, ymin, ymax);
  draw_padding();
  show_window((char*) fbuf);
  while(1);
}

#undef RES
