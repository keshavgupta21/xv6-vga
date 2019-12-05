#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define WINDOW_WIDTH 100
#define WINDOW_HEIGHT 95

char fbuf[WINDOW_HEIGHT][WINDOW_WIDTH];
char __attribute((unused)) discard;

void main(void) {
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
      for (int j = 0; j < WINDOW_WIDTH; j++) {
          fbuf[i][j] = 0x00;
      }
  }
  show_window((char*) fbuf);
  read(0, &discard, 1);
  close_window();
  exit(0);
}
