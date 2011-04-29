#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "syscall.h"
#include "cpu.h"

// Turns MIPS-style named instructions into their numeric counterpart
// in the same way that SPIM does.
static int32_t inline get_reg(char let, int32_t num)
{
	switch (toupper(let)) {
	case 'A': return num + A_REG;

	case 'V': return num + V_REG;

	case 'T': {
		if (num < 7) return num + LT_REG;
		else if (num >= 8 && num <= 9)
			return num + HT_REG;
	} break;

	case 'S': return num + S_REG;

	case 'K': return num + K_REG;
	}
	return -1;
}


void sysc_op(cpu_core *cpu)
{
	// perform one of n syscall operations. including putc, puts, exit, etc.
	if (cpu->verbose) printf("syscall %-2d: ", cpu->registers[get_reg('v', 0)]);
	switch (cpu->registers[get_reg('v', 0)]) {
	case 1:
		// Print the integer contained in A0
		printf("%d", cpu->registers[get_reg('a', 0)]);
		break;

	case 4:
		// print string starting at address contained in A0 (FIXME: this does not increment memory counters!)
		printf("%s", (const char *)cpu->mem->crackaddr(cpu->registers[get_reg('a', 0)]));
		break;

	case 5:
		// read integer from stdin and put it in V0
		throw "5 under construction";
		break;

	case 8: {
		// read string, and put it into memory starting at A0 and running for up to A1 bytes
		int32_t count = 0, c = 0;
		int32_t length = cpu->registers[get_reg('a', 1)];
		// Do things the hard way so that memory statistics are right.
		// (could just crackaddr and write directly, since virtual addresses are mem-mapped into this process)
		while ((c = getchar()) && count < (length - 1)) {
			cpu->mem->set<byte>(cpu->registers[get_reg('a', 0)] + count++, c);
			if (c == 0x0a) break;                                           // on newline, break so that we mimic 'gets'
		}
		cpu->mem->set<byte>(cpu->registers[get_reg('a', 0)] + count++, 0); // add on null char
	} break;

	case 10:
		cpu->usermode = false;
		break;

	case 20:
		// Extension: print the register file to screen.
		for (int32_t x = 0; x < 16; x++) {
			printf("$%d:\t0x%08x\t\t$%d:\t0x%08x\n", x, cpu->registers[x], x + 16, cpu->registers[x + 16]);
		}
		break;

	default:
		throw "Unsupported SYSCALL was performed";
		break;
	}
	printf("\n");
}