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
uint8 readport(uint32 port, uint32 index);
void writeport(uint32 port, uint32 index, uint8 val);
void config_vga();
void read_vga_config();

void vga_init() {
  printf("initializing VGA..\n");
  //const uint64 vga_framebuffer = 0x40000000L;

  // for now set mode to default from vga.h i.e. 80x25 text mode
  config_vga();
  printf("completed VGA initialization.\n");
  read_vga_config();
}

uint8 readport(uint32 port, uint32 index) {
  if (index == 0xff) {
    // if index is 0xff then read directly from the port
    return *(uint8*)(0x3000000L + port);
  } else {
    // write the index to the port.
    *(uint8*)(0x3000000L + port) = index;
    // read the value from port+1.
    uint8 read = *(uint8*)(0x3000000L + port + 1);

    if (port == 0x3c0) {
      volatile uint8 discard __attribute__((unused)) = *(uint8*)(0x3000000L + 0x3da);
    }

    return read;
  }
}

void writeport(uint32 port, uint32 index, uint8 val) {
  if (index == 0xff) {
    // if index is 0xff then write directly to the port
    *(uint8*)(0x3000000L + port) = val;
  } else {
    if (port == 0x3c0) {
      volatile uint8 discard __attribute__((unused)) = *(uint8*)(0x3000000L + 0x3da);
    }
    // write the index to the port.
    *(uint8*)(0x3000000L + port) = index;
    // write the value to port+1.
    if (port == 0x3c0) {
      *(uint8*)(0x3000000L + port) = val;
    } else {
      *(uint8*)(0x3000000L + port + 1) = val;
    }
  }
}

void config_vga() {
  for (int i = 0; i < 29; i++) {
    writeport(vga_conf_port[i], vga_conf_index[i], vga_conf_value[i]);
  }
}

void read_vga_config() {
  for (int i = 0; i < 29; i++) {
    printf("Port: %x, Index: %x, Value: %x\n", vga_conf_port[i], vga_conf_index[i], readport(vga_conf_port[i], vga_conf_index[i]));
  }
}