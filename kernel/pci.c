//
// simple PCI-Express initialization, probably only
// works for qemu -device VGA
//
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

void
pci_init()
{
  // we'll place the VGA framebuffer at this address.
  uint64 vga_framebuffer = 0x40000000L;

  // qemu -machine virt puts PCIe config space here.
  uint32  *ecam = (uint32 *) 0x30000000L;
  
  // look at each device on bus 0.
  for(int dev = 0; dev < 32; dev++){
    int bus = 0;
    int func = 0;
    int offset = 0;
    uint32 off = (bus << 16) | (dev << 11) | (func << 8) | (offset);
    volatile uint32 *base = ecam + off;
    uint32 id = base[0];
    
    if(id == 0x11111234){
      // PCI device ID 1111:1234 is VGA
      
      // command and status register.
      // bit 0 : I/O access enable
      // bit 1 : memory access enable
      // bit 2 : enable mastering
      base[1] = 7;
      __sync_synchronize();

      for(int i = 0; i < 6; i++){
        uint32 old = base[4+i];

        // writing all 1's to the BAR causes it to be
        // replaced with its size (!).
        base[4+i] = 0xffffffff;
        __sync_synchronize();

        base[4+i] = old;
      }

      // tell the VGA to reveal its framebuffer at
      // physical address 0x40000000.
      base[4+0] = vga_framebuffer;

      vga_init((char*)vga_framebuffer);
    }
  }

}
