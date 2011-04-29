#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "memory.h"
#include "cpu.h"

extern char * optarg;
extern int32_t optind;
extern int32_t optopt;
extern int32_t opterr;
extern int32_t optreset;

// Usage of the program
static void usage(char * name) {
  std::cout << name << " usage:\n"
            << "\t-t text_stream_file: load .text with the contents of file\n"
            << "\t-d data_stream_file: [optional] load .data with contents of file\n"
            << "\t-v: very verbose single-click CPU (echo every stage, pause after each cycle)\n"
            << "\t-m: collect and display memory statistics\n\t\t(position relative to -t and -d is important)\n";
}


// *****************************
//           entry point
// *****************************
int32_t main(int32_t argc, char ** argv) {
  memory mem;
  bool verb = false;
  bool text_loaded = false;
  bool show_cycles = false;
  int32_t ch;
  uint32_t text_ptr = text_segment;
  uint32_t data_ptr = data_segment;
  while ((ch = getopt(argc, argv, "t:d:vmc")) != -1) {
    switch (ch) {
    case 't': {
        std::ifstream input(optarg, std::ios::binary);
        if (!(input.good() && input.is_open())) { 
          std::cout << *argv << ": " << optarg << " does not exist" << std::endl; 
          exit(20); 
        }
        byte c;
        char * pc = (char*)&c;
        while (!input.eof()) {
          input.read(pc, sizeof(byte));
          mem.set<byte>(text_ptr++, c);
        }
        input.close();
        text_loaded = true;
      }
      break;
    case 'm':
      mem.collect_stats(true);
      break;
    case 'c':
      show_cycles = true;
      break;
    case 'd': {
        std::ifstream input(optarg, std::ios::binary);
        if (!(input.good() && input.is_open())) { 
          std::cout << *argv << ": " << optarg << " does not exist" << std::endl; 
          exit(20); 
        }
        byte c;
        while (!input.eof()) {
          input.read((char*)&c, sizeof(byte));
          mem.set<byte>(data_ptr++, c);
        }
        input.close();
      }
      break;
    case 'v':
      verb = true;
      break;
    default:
      usage(*argv);
      exit(10);
      break;
    }
  }

  if (!text_loaded) {
    usage(*argv);
    exit(10);
  }
  std::cout << *argv << ": Starting CPU..." << std::endl;
  run_cpu(&mem, verb);
  std::cout << *argv << ": CPU Finished" << std::endl;
  
  if (mem.is_collecting())
    mem.display_memory_stats();
}

