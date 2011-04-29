#include "codegen.h"
#include "../sim/types.h"

using namespace std;

// the current location is noted as a label. go back and write in the right
// places. memorize for future emissions of this label.
void codegen::label_here(string name)
{
	streampos position(current->out->tellp());

	labels[name] = writeback_position_t(position, current->out);

	// scan the pending_balance map for all locations that need updating
	pair<pending_balance_t::iterator, pending_balance_t::iterator> ranges(pending_balance.equal_range(name));

	pending_balance_t::iterator i = ranges.first;
	pending_balance_t::iterator end = ranges.second;

	while (i != end) {
		ostream & out(*(i->second.out));
		streampos oldpos(out.tellp());
		out.seekp(i->second.position);
		uint32_t abspos(position + current->position);
		out.write((char *)&abspos, sizeof(uint32_t));
		out.seekp(oldpos);
		++i;
	}

	ranges = pending_balance.equal_range(name);
	pending_balance.erase(ranges.first, ranges.second);
}


// if label is already defined, just write address and we are done
// if not defined, when the desired label is found, the appropriate
// address will be written at location fileloc into ostream file.
void codegen::emit_label_address(string name)
{
	const uint32_t blank = 0x0badf00d;

	if (labels.find(name) == labels.end()) {
		writeback_position_t s(current->out->tellp(), current->out);
		pending_balance.insert(pair<string, writeback_position_t>(name, s));
		current->out->write((char *)&blank, sizeof(uint32_t));
	}
	else {
		uint32_t container = labels[name].position;
		current->out->write((char *)&container, sizeof(uint32_t));
	}
}