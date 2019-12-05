#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BACKGROUND_COLOR 0xf0
#define BALL_COLOR 0xff
#define PAD_COLOR 0xff
#define WINDOW_WIDTH 100
#define WINDOW_HEIGHT 95
#define PADDING_SIZE 1
#define BALL_WIDTH 5

char fbuf[WINDOW_HEIGHT][WINDOW_WIDTH];
char __attribute((unused)) discard;
int ball_center_x, ball_center_y;

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

static int square_distance(int y, int x) {
  return x * x + y * y;
}

static void draw_ball() {
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
    for (int j = 0; j < WINDOW_WIDTH; j++) {
      if (square_distance(i - ball_center_y, j - ball_center_x) < BALL_WIDTH * BALL_WIDTH) {
        fbuf[i][j] = BALL_COLOR;
      }
    }
  }
  return;
}

void main(void) {
  draw_background();
  draw_padding();

  ball_center_x = WINDOW_WIDTH / 2;
  ball_center_y = WINDOW_HEIGHT / 2;
  draw_ball();

  show_window((char*) fbuf);
  read(0, &discard, 1);
  close_window();
  exit(0);
}
