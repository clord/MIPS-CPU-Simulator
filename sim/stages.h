#ifndef _LATCH_H_
#define _LATCH_H_
#include "instruction.h"

class latch {
public:
  byte opcode;
  inline const instruction * control() {
    return &instructions[opcode];
  }
};

class IDl : public latch {
public:
  byte Rdest, Rsrc1, Rsrc2;
  system_word immediate;
	system_word PC;
	bool predict_taken;
};

class DEl : public latch {
public:
  byte Rdest, Rsrc1, Rsrc2;
  system_word immediate;
  int Rsrc1Val, Rsrc2Val;
	system_word PC;
	bool predict_taken;
};

class EMl : public latch {
public:
  byte Rdest, Rsrc1, Rsrc2;
  system_word aluresult;
  int Rsrc1Val, Rsrc2Val;
};

class MWl : public latch {
public:
	system_word aluresult, mem_data, Rdest;
	int Rsrc2Val, Rsrc1Val;
};

class InstructionFetchStage {
public:	
  cpu_core * core;
  IDl right;
  InstructionFetchStage(cpu_core * c){core = c; make_nop();}
  void Execute();
	void make_nop(){
		bzero(&right, sizeof(IDl));
	}
};

class InstructionDecodeStage {
public:	
  cpu_core * core;
  IDl left;
  DEl right;
  InstructionDecodeStage(cpu_core * c){core = c; make_nop(); }
  void Execute();
	void Shift();
	void Resolve();
	void make_nop(){
		bzero(&left, sizeof(IDl)); 
		bzero(&right, sizeof(DEl));
	}
	
};

class ExecuteStage {
public:	
  cpu_core * core;
  DEl left;
  EMl right;
  ExecuteStage(cpu_core * c){core = c; make_nop(); }
  void Execute();
	void Shift();
	void Resolve();
	void make_nop(){
		bzero(&left, sizeof(DEl)); 
		bzero(&right, sizeof(EMl));
	}
	
};

class MemoryStage {
public:	
  cpu_core * core;
  EMl left;
  MWl right;
  MemoryStage(cpu_core * c){core = c; make_nop(); }
  void Execute();
	void Shift();
	void Resolve();
	void make_nop(){
		bzero(&right, sizeof(MWl)); 
		bzero(&left, sizeof(EMl));
	}
	
};

class WriteBackStage {
public:	
  cpu_core * core;
  MWl left;
  WriteBackStage(cpu_core * c){core = c; make_nop();}
  void Execute();
	void Shift();
	void make_nop(){
		 bzero(&left, sizeof(MWl));
	}
	
};

#endif /* _LATCH_H_ */
