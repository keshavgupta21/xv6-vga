//
// empty VGA driver.
// qemu ... -device VGA -display cocoa
//
// pci.c maps the VGA framebuffer at 0x40000000 and
// passes that address to vga_init().
//
// vm.c maps the VGA "IO ports" at 0x3000000.
//
// we're talking to hw/display/vga.c in the qemu source.
// you can modify that file to generate debugging output.
//
// http://www.osdever.net/FreeVGA/home.htm
// https://wiki.osdev.org/VGA_Hardware
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "vga.h"
#include "palette.h"

#define C(x)  ((x)-'@')  // Control-x

#define NO_KEYCB 0xffffffffffffffffull

uint8 readport(uint32 port, uint8 index);
void writeport(uint32 port, uint8 index, uint8 val);


volatile uint8 __attribute__((unused)) discard; // write to this to discard
char * vga_buf;

int selected_win = -1;

window_t windows[6] = {{.pid = -1, .key_cb = NO_KEYCB},
                       {.pid = -1, .key_cb = NO_KEYCB},
                       {.pid = -1, .key_cb = NO_KEYCB},
                       {.pid = -1, .key_cb = NO_KEYCB},
                       {.pid = -1, .key_cb = NO_KEYCB},
                       {.pid = -1, .key_cb = NO_KEYCB}};

static volatile uint8 * const VGA_BASE = (uint8*) 0x3000000L;

void vga_init(char * vga_framebuffer) {
  // printf("initializing VGA..\n");
  vga_buf = vga_framebuffer;

  // Load graphics mode config
  vga_config_t * vga_config = vga_config_img_320_300;
  for (int i = 0; i < 56; i++) {
    writeport(vga_config[i].port, vga_config[i].index, vga_config[i].val);
    //printf("Writing at port %x index %x value %x\n", vga_config[i].port, vga_config[i].index, vga_config[i].val);
  }

  // configure a custom VGA palette
  for (int i = 0; i < 256; i++) {
    std_palette[i] = 0;
    std_palette[i] |= ((i & 0xc0)) << 16;
    std_palette[i] |= ((i & 0x38) << 2) << 8;
    std_palette[i] |= ((i & 0x07) << 5);
  }
  std_palette[255] = 0xfcfcfc;

  // Set default VGA palette
  writeport(0x3c8, 0xff, 0x00);
  for (int i = 0; i < 256; i++) {
    writeport(0x3c9, 0xff, (std_palette[i] & 0xfc0000) >> 18);
    writeport(0x3c9, 0xff, (std_palette[i] & 0x00fc00) >> 10);
    writeport(0x3c9, 0xff, (std_palette[i] & 0x0000fc) >> 2);
  }

  // set the background
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      vga_buf[y * WIDTH + x] = BACKGROUND;
    }
  }

  printf("completed VGA initialization.\n");
}

uint8 readport(uint32 port, uint8 index) {
  uint8 read;
  discard = VGA_BASE[0x3da];
  switch (port) {
    case 0x3c0:
      VGA_BASE[0x3c0] = index;
      read = VGA_BASE[0x3c1];
    break;
    case 0x3c2:
      read = VGA_BASE[0x3cc];
    break;
    case 0x3c4:
    case 0x3ce:
    case 0x3d4:
      VGA_BASE[port] = index;
      read = VGA_BASE[port + 1];
    break;
    case 0x3d6:
      read = VGA_BASE[0x3d6];
    break;
    case 0x3c9:
      read = VGA_BASE[0x3c9];
    break;
    default:
      read = 0xff;
    break;
  }
  discard = VGA_BASE[0x3da];
  return read;
}

void writeport(uint32 port, uint8 index, uint8 val) {
  discard = VGA_BASE[0x3da];
  switch (port) {
    case 0x3c0:
      VGA_BASE[0x3c0] = index;
      VGA_BASE[0x3c0] = val;
    break;
    case 0x3c2:
      VGA_BASE[0x3c2] = val;
    break;
    case 0x3c4:
    case 0x3ce:
    case 0x3d4:
      VGA_BASE[port] = index;
      VGA_BASE[port + 1] = val;
    break;
    case 0x3d6:
      VGA_BASE[0x3d6] = val;
    break;
    case 0x3c7:
    case 0x3c8:
    case 0x3c9:
      VGA_BASE[port] = val;
    break;
  }
  discard = VGA_BASE[0x3da];
}

// WINDOW MANAGER FUNCTIONALITY


void render_window(int win_loc) {
  int x0 = (win_loc % 3) * (WINDOW_WIDTH + WINDOW_PAD);
  int y0 = (win_loc / 3) * (WINDOW_HEIGHT + WINDOW_PAD);
  
  for (int y = y0; y < y0 + WINDOW_HEIGHT; y++) {
    for (int x = x0; x < x0 + WINDOW_WIDTH; x++) {
      vga_buf[y * WIDTH + x] = windows[win_loc].fbuf[(y - y0)*WINDOW_WIDTH + (x - x0)];
    }
  }
}

uint64 sys_show_window() {
  uint64 fbuf_usr;
  if (argaddr(0, &fbuf_usr) < 0) { return -1; }
  struct proc * p = myproc();
  int win_loc = -1, empty_loc = -1;
  for (int i = 5; i >= 0; i--) {
    if (windows[i].pid == p->pid) {
      win_loc = i;
    }
    if (windows[i].pid == -1) {
      empty_loc = i;
    }
  }
  if (win_loc == -1) {
    if (empty_loc == -1) {
      return -1;
    } else {
      win_loc = empty_loc;
      selected_win = win_loc;
      printf("controlling window %d\n", selected_win);
      windows[win_loc].pid = p->pid;
      windows[win_loc].proc = p;
    }
  } 
  copyin(p->pagetable, windows[win_loc].fbuf, fbuf_usr, WINDOW_WIDTH * WINDOW_HEIGHT);

  render_window(win_loc);

  return 0;
}

uint64 sys_close_window() {
  struct proc * p = myproc();
  for (int win_loc = 5; win_loc >= 0; win_loc--) {
    if (windows[win_loc].pid == p->pid) {
      windows[win_loc].pid = -1;
      if (win_loc == selected_win) {
        for (int i = 0; i < 6; i++) {
          if (windows[i].pid != -1) {
            selected_win = i;
            printf("controlling window %d\n", i);
          }
        }
        if (selected_win == -1) {
          printf("no window is controlled\n");
        }
      }
      for (int i = 0; i < WINDOW_HEIGHT; i++) {
        for (int j = 0; j < WINDOW_WIDTH; j++) {
          windows[win_loc].fbuf[i*WINDOW_WIDTH + j] = BACKGROUND;
        }
      }
      render_window(win_loc);
      return 0;
    }
  }
  return -1;
}

uint64 sys_reg_keycb() {
  // printf("reg cb called from pid %d\n", myproc()->pid);
  uint64 keycbaddr;
  argaddr(0, &keycbaddr);
  struct proc * p = myproc();
  for (int win_loc = 5; win_loc >= 0; win_loc--) {
    if (windows[win_loc].proc == p) {
      // printf("setting window %d keycb to %p\n", win_loc, keycbaddr);
      windows[win_loc].key_cb = keycbaddr;
      return 0;
    }
  }
  int empty_loc = -1;
  for (int i = 5; i >= 0; i--) {
    if (windows[i].pid == -1) {
      empty_loc = i;
    }
  }
  if (empty_loc == -1) {
    return -1;
  } else {
    windows[empty_loc].pid = p->pid;
    windows[empty_loc].proc = p;
    windows[empty_loc].key_cb = keycbaddr;
    // printf("reserved window %d\n", empty_loc);
    selected_win = empty_loc;
  }
  return 0;
}

void saveregs(struct proc *p) {
    p->cb.epc = p->tf->epc;
    p->cb.ra = p->tf->ra;
    p->cb.sp = p->tf->sp;
    p->cb.gp = p->tf->gp;
    p->cb.tp = p->tf->tp;
    p->cb.t0 = p->tf->t0;
    p->cb.t1 = p->tf->t1;
    p->cb.t2 = p->tf->t2;
    p->cb.s0 = p->tf->s0;
    p->cb.s1 = p->tf->s1;
    p->cb.a0 = p->tf->a0;
    p->cb.a1 = p->tf->a1;
    p->cb.a2 = p->tf->a2;
    p->cb.a3 = p->tf->a3;
    p->cb.a4 = p->tf->a4;
    p->cb.a5 = p->tf->a5;
    p->cb.a6 = p->tf->a6;
    p->cb.a7 = p->tf->a7;
    p->cb.s2 = p->tf->s2;
    p->cb.s3 = p->tf->s3;
    p->cb.s4 = p->tf->s4;
    p->cb.s5 = p->tf->s5;
    p->cb.s6 = p->tf->s6;
    p->cb.s7 = p->tf->s7;
    p->cb.s8 = p->tf->s8;
    p->cb.s9 = p->tf->s9;
    p->cb.s10 = p->tf->s10;
    p->cb.s11 = p->tf->s11;
    p->cb.t3 = p->tf->t3;
    p->cb.t4 = p->tf->t4;
    p->cb.t5 = p->tf->t5;
    p->cb.t6 = p->tf->t6;
}

void restoreregs(struct proc *p) {
    p->tf->epc = p->cb.epc;
    p->tf->ra = p->cb.ra;
    p->tf->sp = p->cb.sp;
    p->tf->gp = p->cb.gp;
    p->tf->tp = p->cb.tp;
    p->tf->t0 = p->cb.t0;
    p->tf->t1 = p->cb.t1;
    p->tf->t2 = p->cb.t2;
    p->tf->s0 = p->cb.s0;
    p->tf->s1 = p->cb.s1;
    p->tf->a0 = p->cb.a0;
    p->tf->a1 = p->cb.a1;
    p->tf->a2 = p->cb.a2;
    p->tf->a3 = p->cb.a3;
    p->tf->a4 = p->cb.a4;
    p->tf->a5 = p->cb.a5;
    p->tf->a6 = p->cb.a6;
    p->tf->a7 = p->cb.a7;
    p->tf->s2 = p->cb.s2;
    p->tf->s3 = p->cb.s3;
    p->tf->s4 = p->cb.s4;
    p->tf->s5 = p->cb.s5;
    p->tf->s6 = p->cb.s6;
    p->tf->s7 = p->cb.s7;
    p->tf->s8 = p->cb.s8;
    p->tf->s9 = p->cb.s9;
    p->tf->s10 = p->cb.s10;
    p->tf->s11 = p->cb.s11;
    p->tf->t3 = p->cb.t3;
    p->tf->t4 = p->cb.t4;
    p->tf->t5 = p->cb.t5;
    p->tf->t6 = p->cb.t6;
}

int send_to_console = 1, select_window = 0;

uint64 window_intr(int c) {
  if (c == C('Z')) {
    send_to_console = 0;
    return 1;
  }
  struct proc *p = windows[selected_win].proc;
  if (c == C('X')) {
    p->killed = 1;
    return 1;
  }
  if (c == C('N')) {
    select_window = 1;
    return 1;
  }
  if (select_window) {
    if (selected_win >= 0) {
      select_window = 0;
      selected_win = c - '0';
      if (selected_win >= 6) {
        selected_win = 6;
      } else if (selected_win < 0) {
        selected_win = 0;
      }
      printf("switched to window %d\n", selected_win);
    } else {
      printf("no windows contolled!\n");
    }
    return 1;
  }
  if (send_to_console) {
    return 0;
  }
  // printf("selected_win = %d, p = %p, pid = %d, p->cb.entered = %d, windows[selected_win].key_cb = %p, key = %d\n",
  //        selected_win, p, windows[selected_win].pid, p->cb.entered, windows[selected_win].key_cb, c);
  if (selected_win >= 0 && !p->cb.entered && windows[selected_win].key_cb != NO_KEYCB) {
    saveregs(p);
    printf("going to handler %p\n", windows[selected_win].key_cb);
    p->tf->a1 = (uint64)c;
    p->tf->epc = windows[selected_win].key_cb;
    p->cb.entered = 1;
  }
  send_to_console = 1;
  return 1;
}

uint64 sys_cb_return() {
  struct proc *p = myproc();
  printf("cb return pid = %d\n", p->pid);
  restoreregs(p);
  p->cb.entered = 0;
  return 0;
}