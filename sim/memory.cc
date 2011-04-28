#include "memory.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <iostream>
//#define MAP_ANON MAP_ANONYMOUS

memory::memory() {
  // Allocate some paged memory. All of this is to work around 
  // the poor 32-bit machine that can't mmap a true-flat main memory. 
  // TODO: kernel memory
  text_mem  = (byte*)mmap(NULL, 0x0FC00000,  PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
  if (text_mem == MAP_FAILED) {
    perror(".text mmap failed");
    exit(20);
  }
  data_mem  = (byte*)mmap(NULL, 0x10000000,  PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
  if (data_mem == MAP_FAILED) {
    perror(".data mmap failed");
    exit(20);
  }
  stack_mem = (byte*)mmap(NULL, 0x10000000,  PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
  if (stack_mem == MAP_FAILED) {
    perror(".stack mmap failed");
    exit(20);
  }
  sp = stack_segment;
  collectstats = false;
  
  readhits    = 0; 
  writehits   = 0; 
  bytesin     = 0; 
  bytesout    = 0;
  stackpushes = 0; 
  stackpops   = 0;
}

memory::~memory() {
  munmap(text_mem,  0x0FC00000);
  munmap(data_mem,  0x10000000);
  munmap(stack_mem, 0x10000000);
}

void memory::display_stack() {
  std::cout << "-----stack-----" << std::endl;
  std::cout.setf(std::ios::hex , std::ios::basefield);
  for (int x = stack_segment; x > sp; x-=sizeof(system_word)) {
    std::cout << "0x" << x << ": 0x"
              << *(int*)crackaddr(x) << std::endl;
  }
  std::cout << "---------------" << std::endl;
}


bool memory::is_collecting() {
  return collectstats;
}

void memory::collect_stats(bool state) {
  collectstats = state;
}


void memory::display_memory_stats() {
  if (collectstats)
  std::cout << std::endl  
            << "-=-=-=-=Memory Access Statistics-=-=-=-=" << std::endl  
            << "    read bytes: " << bytesout << std::endl
            << "         reads: " << readhits << " (plus " << stackpops << " pops)"  << std::endl
            << "    bytes/read: " << bytesout/(double)(readhits+stackpops) << std::endl
            << "----------------------------------------" << std::endl  
            << "   write bytes: " << bytesin   << std::endl
            << "        writes: " << writehits << " (plus " << stackpushes << " pushes)" << std::endl
            << "   bytes/write: " << bytesin/(double)(writehits+stackpushes) << std::endl
            << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
  else
  std::cout << "no statistics collected" << std::endl;

}


byte* memory::crackaddr(system_word addr) {
  // Determine which page to use.
  // [0x00000000-0x00400000) is not mapped, and will throw
  // [0x00400000-0x10000000) goes to text
  // [0x10000000-0x50000000) goes to data
  // [0x50000000-0x80000000) goes to stack
  // kernel segments are unmapped, and will also throw

  if (addr < 0x00400000 || addr >= 0x80000000)
    throw "protected memory exception";
  else if (addr < 0x10000000 && addr >= 0x00400000)
    return text_mem + addr;
  else if (addr < 0x50000000 && addr >= 0x10000000)
    return data_mem + addr;
  else if (addr < 0x80000000 && addr > 0x50000000)
    return stack_mem + (0x80000000-addr); // convert to growing up behind the scenes for mmap
  else {
    throw "memory out of range exception";
  }
}

