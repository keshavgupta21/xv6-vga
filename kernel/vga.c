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

// maybe this is the right way to read VGA registers
// that require the index to be written to one port,
// and the value to be read from the next higher port.
// the "IO ports" are actually mapped into memory
// at 0x3000000.
uint8
readport(uint32 port, uint32 index)
{
  // write the index to the port.
  *(uint8*)(0x3000000L + port) = index;

  // read the value from port+1.
  unsigned int x = *(uint8*)(0x3000000L + port + 1);

  return x;
}

void
vga_init(char *framebuffer)
{
  printf("vga_init\n");

  // print some random VGA register values to get some
  // confidence that we're actually talking to the
  // qemu's emulated PCI VGA hardware. these trigger
  // plausible-looking debug output from qemu's
  // hw/display/vga.c.
  printf("0x3c0 0x10 = %x\n", readport(0x3c0, 0x10));
  printf("0x3c0 0x11 = %x\n", readport(0x3c0, 0x11));
  printf("0x3c4 0x01 = %x\n", readport(0x3c4, 0x01));
  printf("0x3c4 0x03 = %x\n", readport(0x3c4, 0x03));
  printf("0x3c4 0x04 = %x\n", readport(0x3c4, 0x04));
  printf("0x3ce 0x05 = %x\n", readport(0x3ce, 0x05));
  printf("0x3ce 0x06 = %x\n", readport(0x3ce, 0x06));
  printf("0x3d4 0x17 = %x\n", readport(0x3d4, 0x17));
}
