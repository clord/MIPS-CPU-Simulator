#ifndef _CPU_H_
#define _CPU_H_
#include "memory.h"
#include "instruction.h"
#include "stages.h"

// Register machine core state.
class cpu_core {
public:
	cpu_core() : ifs(this), ids(this), exs(this), mys(this), wbs(this) {}


	uint32_t PC;
	bool usermode, verbose;
	memory  *mem;
	uint32_t registers[32];
	InstructionFetchStage  ifs;
	InstructionDecodeStage ids;
	ExecuteStage   exs;
	MemoryStage    mys;
	WriteBackStage wbs;
};

void run_cpu(memory *m, const bool verbose);

#endif /* _CPU_H_ */