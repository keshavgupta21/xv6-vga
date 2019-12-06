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

#define UP_ARROW 24
#define DOWN_ARROW 25
#define RIGHT_ARROW 26
#define LEFT_ARROW 27

char fbuf[WINDOW_HEIGHT][WINDOW_WIDTH];
char __attribute((unused)) discard;

int ball_center_x, ball_center_y;
int ball_vel_x, ball_vel_y; // in pixels

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

static void flip_vel_x() {
  ball_vel_x *= -1;
  return;
}

static void flip_vel_y() {
  ball_vel_y *= -1;
  return;
}

static void update_center() {
  ball_center_x += ball_vel_x;
  if (ball_center_x - BALL_WIDTH <= 0 ||
      ball_center_x + BALL_WIDTH >= WINDOW_WIDTH) {
    flip_vel_x();
  }
  ball_center_y += ball_vel_y;
  if (ball_center_y - BALL_WIDTH <= 0 ||
      ball_center_y + BALL_WIDTH >= WINDOW_HEIGHT) {
    flip_vel_y();
  }
  return;
}

static int abs(int x) {
  if (x >= 0) {
    return x;
  }
  return -x;
}

void key_handler(char key) {
  if (key == UP_ARROW) {
    ball_vel_x = abs(ball_vel_x);
  }
  else if (key == DOWN_ARROW) {
    ball_vel_x = -abs(ball_vel_x);
  }
  else if (key == RIGHT_ARROW) {
    ball_vel_y = abs(ball_vel_y);
  }
  else if (key == LEFT_ARROW) {
    ball_vel_y = -abs(ball_vel_y);
  }
}

void main(void) {
  ball_center_x = WINDOW_WIDTH / 2;
  ball_center_y = WINDOW_HEIGHT / 2;
  ball_vel_x = 2;
  ball_vel_y = 1;

  while (1) {
    draw_background();
    draw_padding();
    draw_ball();
    show_window((char*) fbuf);

    sleep(1);
    update_center();
  }


  
  read(0, &discard, 1);
  close_window();
  exit(0);
}
