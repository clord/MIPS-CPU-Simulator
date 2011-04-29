#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <string>
#include <iostream>
#include <map>
#include "../sim/types.h"
#include "memory.h"

struct word__stream {
  uint32_t word;
  std::ostream * str;
};
/*
maintains labels and their locations, as well as where labels have to be updated. 
Writes to the base stream
*/
class codegen {
  uint32_t toff, doff, off;
  std::ostream * t, * d, *writestr;
  std::multimap<std::string, word__stream> pending_find;
  std::map<std::string, word__stream> labels;

public:

  codegen(std::ostream * text, std::ostream * data, uint32_t textoff, uint32_t dataoff);

  void in_text_section();
  void in_data_section();

  template <class T> void emit(T val) {
    writestr->write((char*)&val, sizeof(T));
  }
  void emit_n_bytes(int32_t count);
  void emit_string(char* str, int32_t len);

  // the current location is noted as a label. go back and write in the right
  // places. memorize for future emissions of this label.
  void label_here(std::string name);
  
  // if label is already defined, just return address and we are donee
  // if not defined, when the desired label is found, the appropriate 
  // address will be written at location fileloc into ostream file.
  void emit_label_address(std::string name);
  
  // When the file is finished, use this method to see if all labels
  // are "balanced". If not, you should indicate an error and exit from the app
  bool balanced_labels();
};



#endif /* _CODEGEN_H_ */

