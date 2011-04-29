#include "codegen.h"
#include "../sim/types.h"


codegen::codegen(std::ostream * text, std::ostream * data, uint32_t textoff, uint32_t dataoff) {
  t = text;
  d = data;
  toff = textoff;
  doff = dataoff;
  off = toff;
  writestr = t;
}

void codegen::in_text_section() {
  writestr = t;
  off = toff;
}

void codegen::in_data_section() {
  writestr = d;
  off = doff;
}

void codegen::emit_n_bytes(int32_t count) {
  byte b = 0;
  for (int32_t x = 0; x < count; x++){
    writestr->write((char*)&b, sizeof(byte));
  }
}

void codegen::emit_string(char* str, int32_t len) {
  writestr->write(str, len);
}

// the current location is noted as a label. go back and write in the right
// places. memorize for future emissions of this label.
void codegen::label_here(std::string name) {
  uint32_t position = (uint32_t)writestr->tellp();
  word__stream s = {position + off, writestr};
  labels[name] = s;
  // scan the pending_find map for all locations that need updating
  std::pair<
    std::multimap<std::string,word__stream>::iterator,
    std::multimap<std::string,word__stream>::iterator> 
  ranges = pending_find.equal_range(name);
  
  std::multimap<std::string,word__stream>::iterator i = ranges.first;
  std::multimap<std::string,word__stream>::iterator end = ranges.second;
  
  for(; i!=end; i++) {
    long oldpos = (*i).second.str->tellp();
    (*i).second.str->seekp((*i).second.word);
    uint32_t abspos = position + off;
    (*i).second.str->write((char*)&abspos, sizeof(uint32_t));
    (*i).second.str->seekp(oldpos);
  }  
  ranges = pending_find.equal_range(name);
  pending_find.erase(ranges.first, ranges.second);
}

// if label is already defined, just write address and we are done
// if not defined, when the desired label is found, the appropriate
// address will be written at location fileloc into ostream file.
void codegen::emit_label_address(std::string name) {
  if (labels.find(name) == labels.end()) {
    word__stream s = {(uint32_t)writestr->tellp(), writestr};
    pending_find.insert(std::pair<std::string,word__stream>(name,s));
    uint32_t blank = 0xffffffff;
    writestr->write((char*)&blank, sizeof(uint32_t));
  } else {
    writestr->write((char*)&labels[name].word, sizeof(uint32_t));
  }
}

// When the file is finished, use this method to see if all labels
// are "balanced". If not, you should indicate an error and exit from the app
// due to a label used but not defined.
bool codegen::balanced_labels() {
  return pending_find.empty();
}

