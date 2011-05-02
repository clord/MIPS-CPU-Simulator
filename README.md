## Introduction

This project simulates the execution of a subset of a 32-bit five-stage [MIPS][4] CPU Pipeline
described in “Computer Organization and Design (COD)” by Patterson & Hennessy.

The code is based on a class project from my university days, in which we built from the ground
up this simulator and assembler. In that class, the various parts were split up such that the whole
package never really came into existence. I came across the various labs in my backup and decided
to finish up the project, and add a very small smattering of cleanup and experience into the mix.

## Usage

Extract the package and type `make`. Realize your build environment needs massaging, go off and fix
this-and-that. Tweak `CC` and `CCC`. Figure out how to install Yacc and Bison. Try `make` again.
This will then construct the assembler `rasm` and the simulator `ssim`.
There is an additional shell script called `regs` that does assembly and simulation in one step.
The assembler `rasm` responds to the following command switches:

* `-f sourcefile` – assemble ASCII assembly code from file (required)
* `-t textstreamfile` – override default destination and output binary `.text` to specified file
* `-d datastreamfile` – override default destination and output binary `.data` to specified file

The simulator `rsim` responds to the following command switches:

* `-t textstreamfile` – load memory segment .text with the contents of a binary file (required)
* `-d datastreamfile` – load memory segment .data with contents of a binary file
* `-v` – very verbose CPU. Will echo every instruction, and the associated program counter.
* `-m` – collect memory statistics and display a summary after the CPU terminates.
   * Place after `-f`/`-t` in order to not report on the loading the files into memory.
   * Place before one or both in order to include that activity in the report.


## Implementation Notes

The [instruction pipeline][3] implementation is mostly contained in `stages.{c,h}`. I’ve tried to follow
the basic design of the COD MIPS pipeline. This means [control signals][9] and [latches][1] are simulated.

As a result, much of the semantics used to describe the five-stage MIPS CPU in COD is
transferable directly to the simulator, accelerating development. Unfortunately, all
of this extra simulacrum means lower performance and a larger code-base.

I suggest that reader or contributor follow along with COD as I did.


### Simulating Combinatorical Circuits

Simulating asynchronous behaviour using the sequential machine exposed by C is a
somewhat tricky task. The order in which various actions occur is deterministic, so
the relative order of execution is important. I’ve tackled this problem by running through the
pipeline three times per clock, once for execution of the stages, once to resolve
hazards, and one last time to simulate the action of latches being clocked (or shifted)
forward.

The precise order of execution is sometimes important. The best example is in the
resolution of the two read-after-write hazards. These occurs at each writable media,
i.e. memory and register. The memory read stage might attempt to read from an address
that was written in the same clock cycle. This places certain constraints on the order
of execution of the various stages. Having reorderable execute stages is the reason I
decoupled hazard/shift logic from execution.

Timing issues like this can occur; but I was surprised to find that most of the time
sequential order does not matter as much as with regular C code. This order-independent code
happens whenever a latch is involved. Significant reordering of any one of the stages is
possible, as long as the latches get the same values in the end. It would be interesting to
implement the latches with atomic operations and break the pipeline up across an SMP machine.

The most interesting feature of this design is the virtual latch. A latch is a device which
preserves its output, and can atomically and simultaneously change the output to the input on a clock pulse.
There is no such construct exposed to high level programming languages. The way my code simulates
this is to have a “left” and a “right” edge of each stage. When the shift signal occurs, all of the
right-hand edges are transferred to the left edge of the next stage.


### Hazards

#### Forwarding

The forwarding logic has to handle four different hazard situations corresponding to four dependency situations:

1. Execute-Decode for `Rs`: The result of the execute stage’s right latch is required by the decode stage’s right latch for register `Rs`.
2. Memory-Decode for `Rs`: The result of the memory stage’s right latch is required by the decode stage’s right latch for register `Rs`.
3. Execute-Decode for `Rt`: The result of the execute stage’s right latch is required by the decode stage’s right latch for register `Rt`.
4. Memory-Decode for `Rt`: The result of the memory stage’s right latch is required by the decode stage’s right latch for register `Rt`.

#### Pipeline bubble for `lw`

In the five-stage pipeline, there is a potential issue with loading from memory.
The problem is that data comes out of memory one stage after the ALU result is computed,
but `lw` looks like an r-type instruction.
Thus, the forwarding logic can not successfully attach to the memory result:
it forwards the calculated address from the ALU.

The solution I pursued was to bubble the pipeline by one stage whenever the result of a `lw` is needed by
the very next instruction. This bubble allows the normal forwarding logic to handle the situation correctly.
The previous instruction had to be re-issued, so I backed up the program counter by one instruction.


#### Branch Bubbles

Branch miss-predicts are somewhat easier to resolve. I use a very simple [branch predictor][2] (it predicts not-taken every
time.) This will ensure that mis-predicts occur with some frequency.
If a branch miss-prediction is detected (`mispredict = (prediction != branch_taken)`); then the two previous
stages can be abandoned, creating a pipeline bubble. The program counter adjustment mentioned in the previous section
is not required for the “all-not-taken” predictor. This may or may not be the case if a more successful
predictor is to be used (I have not approached the problem for this project).


## System Memory

In the original design, memory for the system was simulated with several mmap segments mapped into the simulator’s process,
representing the simulated .text, .data, etc. Each was a memory map with only a subset of the segment’s true space. Access outside
the subsets led to hardware faults being thrown (i.e., a C++ exception and an aborted simulator)

The original design did this because it had to fit a 32-bit address space into a 32-bit host process.
Thus each memory mapped segment is smaller than the real MIPS equivalent — although they should be sufficient for almost all programs.

In the version I am checking into GitHub, the entire 32-bit address space is mapped, and I require that you build on
a 64-bit machine with a 64-bit C runtime.

In either case, the operating system will lazily map pages as the simulator touches memory.

## The Assembler

The assembler takes an ASCII MIPS [assembly file][6] and produces a binary representation that is executable
by the simulator. Currently, the assembler will generate separate `.t` (text) and `.d` (data) files, which should be loaded
into the correct segment on the simulator by the user. See the usage section for more details.

My assembler parses and lexes with [Bison][8] and [Flex][7]. I made this choice in university, and it reflected the tools I was using
to build a Pascal compiler at the same time for a different class. Using a grammar to express an assembly language led to certain
advantages as the course content changed, and even now all these years later it lets me make changes with ease.

I’d like to take a moment to express my admiration for the MIPS assembly text format. It’s so much more sane than other assembly languages
I’ve dealt with in my career since university.


### Instruction Encoding

In the interest of avoiding a [complicated fetch stage][5], I use a simple structure — eight bytes for each instruction, aligned at 8 bytes.
In order, there is a one-byte opcode, a two-byte operand field, a byte of padding, and space for a four-byte immediate. I leave the
endiness up to the host compiler, meaning that the object files are not portable between simulators and assemblers compiled for different
host architectures.

I’d rather spend my time building the simulator than coming up with a clever binary packing. It would be really
cool if someone managed to make this thing binary-compatible with a real MIPS chip, but my goal here is gain some
understanding of CPU design.


[1]: http://en.wikipedia.org/wiki/Flip-flop_(electronics)
[2]: http://en.wikipedia.org/wiki/Branch_predictor
[3]: http://en.wikipedia.org/wiki/Instruction_pipeline
[4]: http://en.wikipedia.org/wiki/MIPS_architecture
[5]: http://wiki.osdev.org/X86_Instruction_Encoding
[6]: http://en.wikipedia.org/wiki/Assembly_language
[7]: http://en.wikipedia.org/wiki/Flex_lexical_analyser
[8]: http://en.wikipedia.org/wiki/GNU_bison
[9]: http://www.d.umn.edu/~gshute/spimsal/new/control-signal-summary.xhtml





# License

The MIT License

Copyright (c) 2006 Christopher Lord

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
