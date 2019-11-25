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
#include "font.h"

int window_en = 0;

// maybe this is the right way to read VGA registers
// that require the index to be written to one port,
// and the value to be read from the next higher port.
// the "IO ports" are actually mapped into memory
// at 0x3000000.
uint8 readport(uint32 port, uint8 index);
void writeport(uint32 port, uint8 index, uint8 val);
void dump_vga_config(); // for debug

static volatile uint8 * const VGA_BASE = (uint8*) 0x3000000L;
//static volatile uint8 * const VGA_MMIO = (uint8*) KERNBASE + 0xA0000;
volatile uint8 __attribute__((unused)) discard; // write to this to discard

char * vga_buf;

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

  // Set default VGA palette
  writeport(0x3c8, 0xff, 0x00);
  for (int i = 0; i < 256; i++) {
    writeport(0x3c9, 0xff, (std_palette[i] & 0xfc0000) >> 18);
    writeport(0x3c9, 0xff, (std_palette[i] & 0x00fc00) >> 10);
    writeport(0x3c9, 0xff, (std_palette[i] & 0x0000fc) >> 2);
  }

  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 200; y++) {
      vga_buf[y * 320 + x] = (((x + 1)%320)/20)*16 + (y/13);
    }
  }

  printf("completed VGA initialization.\n");
}

void draw_char(char c, int x, int y) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 5; col++) {
      if (VGA_FONT[c * 8 + row] & (1 << (7 - col))) {
        vga_buf[(row + y) * 320 + (col + x)] = 0x00;
      }
    }
  }
}

void show_window(char * text) {
  int n = strlen((const char *) text);
  int w = (n + 2) * 5;
  int h = 18;
  int x0 = 160 - w/2;
  int y0 = 100 - h/2;

  for (int x = x0; x < x0 + w; x++) {
    for (int y = y0; y < y0 + h; y++) {
      vga_buf[y * 320 + x] = 0xff;
    }
  }
  int pos = x0 + 5;
  for (char *c = text; *c != 0; c++) {
    draw_char(*c, pos, y0 + 5);
    pos += 5;
  }
}

uint64 window_intr(int c) {
  if (c == '*') {
    for (int x = 0; x < 320; x++) {
      for (int y = 0; y < 200; y++) {
        vga_buf[y * 320 + x] = (((x + 1)%320)/20)*16 + (y/13);
      }
    }
    // input was used
    return 1;
  } else if (c == '&') {
    show_window("elene machaidze");
    // input was used
    return 1;
  }
  // input was not used
  return 0;
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

void dump_vga_config() {
  vga_config_t * vga_config = vga_config_text_80_25;
  for (int i = 0; i < 56; i++) {
    printf("Port: %x, Index: %x, Value: %x\n", vga_config[i].port, vga_config[i].index, readport(vga_config[i].port, vga_config[i].index));
  }
  printf("Value at 0x3c4, 0x04 might be incorrect\n");
}