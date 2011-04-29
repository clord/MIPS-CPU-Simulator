rasm: scanner.o rasmsemantics.o assembler.o codegen.o
	g++ $(FLAGS) $^ -o rasm

rasmsemantics.o: rasm.tab.cpp rasm.tab.hpp
	g++ $(FLAGS) -c rasm.tab.cpp -o rasmsemantics.o

rasm.tab.cpp rasm.tab.hpp: asm/semantics.ypp
	bison -dvt -b rasm $^

assembler.o: asm/assembler.cc
	g++ $(FLAGS) -c $^

lex.yy.c: asm/scanner.lex rasm.tab.hpp
	flex $^

scanner.o: lex.yy.c
	g++ $(FLAGS) -c lex.yy.c -o scanner.o

codegen.o: asm/codegen.cc asm/codegen.h
	g++ $(FLAGS) -c asm/codegen.cc

