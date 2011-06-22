FLAGS=-g -Wall

all: rsim rasm

include sim/Makefile
include asm/Makefile

clean:
	rm -rf *.o *.d *lex.yy.c *.output *.tab.?pp *.vcg rsim rasm

deliverable: clean
	tar -zcvf clord-mips-cpu-sim.tgz Makefile sim/ asm/ regs *.s

