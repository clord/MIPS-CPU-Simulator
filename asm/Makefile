rasm: scanner.o rasmsemantics.o assembler.o codegen.o
	g++ $(FLAGS) $^ -o rasm

rasmsemantics.o: fixed.rasm.tab.cpp rasm.tab.hpp
	g++ $(FLAGS) -c fixed.rasm.tab.cpp -o rasmsemantics.o

rasm.tab.cpp rasm.tab.hpp: asm/semantics.ypp
	bison -dvt -b rasm $^

assembler.o: asm/assembler.cc
	g++ $(FLAGS) -c $^

lex.yy.c: asm/scanner.lex rasm.tab.hpp
	flex $^

scanner.o: fixed.lex.yy.c
	g++ $(FLAGS) -c fixed.lex.yy.c -o scanner.o

codegen.o: asm/codegen.cc asm/codegen.h
	g++ $(FLAGS) -c asm/codegen.cc

fixed.lex.yy.c: lex.yy.c
	sed "s/find_rule\://g" $^ > fixed.lex.yy.c
# This gets rid of a warning. REMOVE THIS IF YOU USE LEX's REJECT!

fixed.rasm.tab.cpp: rasm.tab.cpp
	sed "s/__attribute__ ((__unused__))//g" $^ > fixed.rasm.tab.cpp
# This is a wonderful hack to work around the old software on the lab machines.
# The g++ compiler here does not support __attribute__ ((__unused__)), which is used
# by bison's codegen tool. solution would be either to remove the attribute, upgrade g++,
# or switch this project over to C...
