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

#define UP_ARROW 'w'
#define DOWN_ARROW 's'
#define RIGHT_ARROW 'd'
#define LEFT_ARROW 'a'
#define ZOOM_IN 'q'
#define ZOOM_OUT 'e'

#define RES (1 << 28)

char fbuf[WINDOW_HEIGHT][WINDOW_WIDTH];

long long xmax = (1*RES)/2, ymax = (5*RES)/4, xmin = -2*RES, ymin = -(5*RES)/4;
volatile int update_window = 1;

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

void draw_brot() {
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

void key_handle(uint64 key0, uint64 key1) {
  long long dx = (xmax - xmin)/10;
  long long dy = (ymax - ymin)/10;
  if (key1 == UP_ARROW) {
    ymin -= dy;
    ymax -= dy;
  } else if (key1 == DOWN_ARROW) {
    ymin += dy;
    ymax += dy;
  } else if (key1 == LEFT_ARROW) {
    xmin -= dx;
    xmax -= dx;
  } else if (key1 == RIGHT_ARROW) {
    xmin += dx;
    xmax += dx;
  } else if (key1 == ZOOM_IN) {
    xmin += dx;
    xmax -= dx;
    ymin += dy;
    ymax -= dy;
  } else if (key1 == ZOOM_OUT) {
    xmin -= dx;
    xmax += dx;
    ymin -= dy;
    ymax += dy;
  }
  // printf("xmin = %d, xmax = %d, ymin = %d, ymax = %d\n", xmin, xmax, ymin, ymax);
  draw_brot();
  draw_padding();
  update_window = 1;
  cb_return();
}

void main(void) {
  reg_keycb(key_handle);
  draw_brot();
  draw_padding();
  while (1) {
    // printf("here\n");
    if (update_window) {
      printf("updating window\n");
      show_window((char*) fbuf);
      update_window = 0;
    }
  }
}

#undef RES
