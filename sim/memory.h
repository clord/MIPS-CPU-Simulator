#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <sys/types.h>
#include <string.h>
#include "types.h"

// some constants that are memory-specific
const system_word  text_segment = 0x00400000;
const system_word  data_segment = 0x10000000;
const system_word ktext_segment = 0x80000080;
const system_word kdata_segment = 0x90000000;
const system_word stack_segment = 0x80000000-0x1000;
// ^ stack grows down, this is the bottom-most element (reserved space for catching errors)
// mmap supports growing down, so all is well.

class memory {

  // pointers to the various system segments.
  byte *  text_mem;
  byte *  data_mem;
  byte * stack_mem;

  int sp;
  
  bool collectstats;

  unsigned int readhits, writehits, bytesin, bytesout, stackpushes, stackpops;

public:

  memory();
  ~memory();

  byte* crackaddr(system_word addr);
  
  void display_stack();
  
  void collect_stats(bool val);
  void display_memory_stats();
  bool is_collecting();
  
  template <class T> void push_stack(T value) {
    memcpy(crackaddr(sp), &value, sizeof(T));
    sp -= sizeof(T);
    if (collectstats) {
      stackpushes++;
      bytesin += sizeof(T);
    }
  }
  
  template <class T> T pop_stack() {
    T ret;
    sp += sizeof(T);
    memcpy(&ret, crackaddr(sp), sizeof(T));
    if (collectstats) {
      stackpops++;
      bytesout += sizeof(T);
    }
    return ret;
  }

  
  // templated to support elements of any size. argument must support sizeof
  // and be byte-addressable. 
  template <class T> T get(system_word addr) {
    byte buff[sizeof(T)];
    memcpy(buff, crackaddr(addr), sizeof(T));
    if (collectstats) {
      readhits++;
      bytesout += sizeof(T);
    }
    return *(T*)buff;
  }

  template <class T> void set(system_word addr, T value) {
    memcpy(crackaddr(addr), &value, sizeof(T));
    if (collectstats) {
      writehits++;
      bytesin += sizeof(T);
    }
  }

};

#endif /* MEMORY */
