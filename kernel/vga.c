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

void vga_init(char * vga_framebuffer) {
  printf("initializing VGA..\n");

  // Load graphics mode config
  vga_config_t * vga_config = vga_config_img_320_300;
  for (int i = 0; i < 56; i++) {
    writeport(vga_config[i].port, vga_config[i].index, vga_config[i].val);
    //printf("Writing at port %x index %x value %x\n", vga_config[i].port, vga_config[i].index, vga_config[i].val);
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
      vga_framebuffer[y * 320 + x] = (x/20)*16 + (y/13);
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

void dump_vga_config() {
  vga_config_t * vga_config = vga_config_text_80_25;
  for (int i = 0; i < 56; i++) {
    printf("Port: %x, Index: %x, Value: %x\n", vga_config[i].port, vga_config[i].index, readport(vga_config[i].port, vga_config[i].index));
  }
  printf("Value at 0x3c4, 0x04 might be incorrect\n");
}