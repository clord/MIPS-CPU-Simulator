#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

// Function pointer for "special" instructions
class cpu_core;
typedef void (*operation)(cpu_core *cpu);
void sysc_op(cpu_core *cpu);

// Information about each instruction in the cpu, including function pointer and a
// character description for debugging purposes.
typedef struct _instruction {
	char name[9];
	operation special_case; // if nonnull handled by some magical process.
	const bool register_write;

	const byte alu_operation;  // an integer which determines which operation the ALU should perform
	const byte alu_source;     // second arg source
	                           // 0: source from register.
	                           // 1: source from immediate.
	                           // 2: source from sum of immidate and register (addrcalc)

	const bool branch;
	const byte mem_write;
	const byte mem_read;
	const bool mem_to_register;
} instruction;

// opcode is the instruction's position in the array.
const instruction instructions[11] =
  {{ "    nop", NULL,     false, 0, 0, false, 0, 0, false }     // 00
  ,{ "   addi", NULL,     true,  1, 1, false, 0, 0, false }     // 01
  ,{ "   beqz", NULL,     false, 0, 0, true,  0, 0, false }     // 02
  ,{ "    bge", NULL,     false, 0, 0, true,  0, 0, false }     // 03
  ,{ "    bne", NULL,     false, 0, 0, true,  0, 0, false }     // 04
  ,{ "     la", NULL,     true,  0, 1, false, 0, 0, false }     // 05
  ,{ "     lb", NULL,     true,  0, 2, false, 0, 1, true  }     // 06
  ,{ "     li", NULL,     true,  0, 1, false, 0, 0, false }     // 07
  ,{ "   subi", NULL,     true,  2, 1, false, 0, 0, false }     // 08
  ,{ "    add", NULL,     true,  1, 0, false, 0, 0, false }     // 09
  ,{ "syscall", &sysc_op, false, 0, 0, false, 0, 0, false } };  // 0a

#endif /* _INSTRUCTION_H_ */