#include "memory.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
//#define MAP_ANON MAP_ANONYMOUS

using namespace std;


memory::memory()
{
	mem = (byte *)mmap(NULL, 0x100000000, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
	if (mem == MAP_FAILED) {
		perror("mmap failed to allocate simulator ram");
		exit(20);
	}
	sp = stack_segment;
	collectstats = false;

	readhits = 0;
	writehits = 0;
	bytesin = 0;
	bytesout = 0;
	stackpushes = 0;
	stackpops = 0;
}


memory::~memory()
{
	munmap(mem, 0x100000000);
}


void memory::display_stack()
{
	cout << "-----stack-----" << endl;
	cout.setf(ios::hex, ios::basefield);
	for (int32_t x = stack_segment; x > sp; x -= sizeof(uint32_t)) {
		cout << "0x" << x << ": 0x" <<
		*(int32_t *)crackaddr(x) << endl;
	}
	cout << "---------------" << endl;
}


bool memory::is_collecting()
{
	return collectstats;
}


void memory::collect_stats(bool state)
{
	collectstats = state;
}


void memory::display_memory_stats()
{
	if (collectstats)
		cout << endl <<
		     "-=-=-=-=Memory Access Statistics-=-=-=-=" << endl <<
		     "    read bytes: " << bytesout << endl <<
		     "         reads: " << readhits << " (plus " << stackpops << " pops)" << endl <<
		     "    bytes/read: " << bytesout / double(readhits + stackpops) << endl <<
		     "----------------------------------------" << endl <<
		     "   write bytes: " << bytesin << endl <<
		     "        writes: " << writehits << " (plus " << stackpushes << " pushes)" << endl <<
		     "   bytes/write: " << bytesin / double(writehits + stackpushes) << endl <<
		     "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
	else
		cout << "no statistics collected" << endl;
}


byte *memory::crackaddr(uint32_t addr)
{
	// TODO: raise exceptions on memory protection faults
	return reinterpret_cast<byte*>(mem + addr);
}
