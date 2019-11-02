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

// maybe this is the right way to read VGA registers
// that require the index to be written to one port,
// and the value to be read from the next higher port.
// the "IO ports" are actually mapped into memory
// at 0x3000000.
uint8 readport(uint32 port, uint8 index);
void writeport(uint32 port, uint8 index, uint8 val);
void dump_vga_config(); // for debug

static volatile uint8 * const VGA_BASE = (uint8*) 0x3000000L;
//const uint64 vga_framebuffer = 0x40000000L;
volatile uint8 __attribute__((unused)) discard; // write to this to discard

void vga_init() {
  printf("initializing VGA..\n");
  // Disable Display

  // Unlock CTRC i.e. CRT Controller (unlock registers)

  // Load Registers (for now set mode to default from vga.h i.e. 80x25 text mode)
  vga_config_t * vga_config = vga_config_text_80_25;
  for (int i = 0; i < 29; i++) {
    writeport(vga_config[i].port, vga_config[i].index, vga_config[i].val);
    printf("Writing at port %x index %x value %x\n", vga_config[i].port, vga_config[i].index, vga_config[i].val);
  }

  // Clear screen

  // Load fonts

  // Lock CTRC i.e. CRT Controller (unlock registers)

  // Enable Display

  printf("completed VGA initialization.\n");
  dump_vga_config();
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
  }
  discard = VGA_BASE[0x3da];
}

void dump_vga_config() {
  vga_config_t * vga_config = vga_config_text_80_25;
  for (int i = 0; i < 29; i++) {
    printf("Port: %x, Index: %x, Value: %x\n", vga_config[i].port, vga_config[i].index, readport(vga_config[i].port, vga_config[i].index));
  }
  printf("Value at 0x3c4, 0x04 might be incorrect\n");
}