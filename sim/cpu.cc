#include <stdio.h>
#include "cpu.h"
#include "memory.h"
#include <unistd.h>

// Some minimal state display. If I had the time, I'd do a quick gui app, which is more natural
// understanding what is going on under the covers.
static void print_stages(cpu_core *core)
{
	printf("0x%08x ", core->PC - 8);
	printf("(if '%s', $%-2d {$%-2d $%-2d} %08x) "
	      , core->ifs.right.control()->name
	      , core->ifs.right.Rdest
	      , core->ifs.right.Rsrc1
	      , core->ifs.right.Rsrc2
	      , core->ifs.right.immediate);

	printf("(id '%s' $%-2d {$%-2d $%-2d}={%08x, %08x} %08x) "
	      , core->ids.left.control()->name
	      , core->ids.right.Rdest
	      , core->ids.right.Rsrc1
	      , core->ids.right.Rsrc2
	      , core->ids.right.Rsrc1Val
	      , core->ids.right.Rsrc2Val
	      , core->ids.right.immediate);

	printf("(ex '%s' $%-2d <= $%-2d%c($%-2d or %08x)) "
	      , core->exs.left.control()->name
	      , core->exs.right.Rdest
	      , core->exs.right.Rsrc1
	      , (core->exs.left.opcode == 1 || 
	        (core->exs.left.opcode == 9) ? '+' : (core->exs.left.opcode == 8 ? '-' : ' '))
	      , core->exs.right.Rsrc2
	      , core->exs.right.aluresult);

	if (core->mys.left.control()->mem_read) {
		printf(" (ms '%s'  read_mem: 0x%0X val: %08x) "
		      , core->mys.left.control()->name
		      , core->mys.left.aluresult, core->mys.right.mem_data);
	}
	else if (core->mys.left.control()->mem_write) {
		printf(" (ms '%s' write_mem: 0x%0X val: %08x) "
		      , core->mys.left.control()->name
		      , core->mys.left.aluresult, core->mys.right.mem_data);
	}
	else if (core->mys.left.control()->branch) {
		printf(" (ms done) ");
	}
	else {
		printf(" (ms '%s') ", core->mys.left.control()->name);
	}

	if (core->wbs.left.control()->register_write) {
		if (core->wbs.left.control()->mem_to_register) {
			printf(" (wb '%-s' $%-2d<=%08x)"
			      , core->wbs.left.control()->name
			      , core->wbs.left.Rdest
			      , core->wbs.left.mem_data);
		}
		else {
			printf(" (wb '%-s' $%-2d<=%08x )"
			      , core->wbs.left.control()->name
			      , core->wbs.left.Rdest
			      , core->wbs.left.aluresult);
		}
	}
	else {
		printf(" (wb '%-s' nowriteback)", core->wbs.left.control()->name);
	}

	printf("\n");
}


// Actual execution of whatever is in the CPU will occur here.
void run_cpu(memory *mem, const bool verbose_cpu)
{
	cpu_core core;

	core.PC = text_segment;
	core.usermode = true;
	core.mem = mem;
	core.verbose = verbose_cpu;

	// initialize registers
	for (int32_t x = 0; x < 32; x++) core.registers[x] = 0;

	// start the cpu loop
	try {
		while (core.usermode) {
			// Execute the stages
			core.mys.Execute();
			core.wbs.Execute(); // First so that writes happen before reads
			core.ifs.Execute();
			core.ids.Execute();
			core.exs.Execute();

			// Fix data hazards and the delay slot due to mem_read.
			core.ids.Resolve();
			core.exs.Resolve();
			core.mys.Resolve();

			// Print the intermediate stages onto the screen for debugging.
			if (core.verbose) {
				print_stages(&core);
			}

			// shift the latches
			core.ids.Shift();
			core.exs.Shift();
			core.mys.Shift();
			core.wbs.Shift();

#if 0
			// Occasionally this stuff is useful
			if (core.verbose) {
				for (int32_t x = 0; x < 8; x++) {
					printf("$%d:\t0x%08x\t\t$%d:\t0x%08x\t\t$%d:\t0x%08x\t\t$%d:\t0x%08x\n"
					      , x, core.registers[x]
					      , x + 8, core.registers[x + 8]
					      , x + 16, core.registers[x + 16]
					      , x + 24, core.registers[x + 24]);
				}
				//getchar(); // slows things down!
			}
#endif
		}
	} catch (const char *e) {
		printf("CPU fault: %s\n", e);
	}
}