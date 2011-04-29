#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <string>
#include <iostream>
#include <map>
#include "../sim/types.h"
#include "memory.h"

struct writeback_position_t {
	std::streampos position;
	std::ostream *out;
	writeback_position_t(std::streampos p, std::ostream *_str) : position(p), out(_str) {}


	writeback_position_t() : position(), out(0) {}
};

// Maintains labels and their locations, as well as where labels have to be updated.
// Writes to the base stream
class codegen {
	typedef std::multimap<std::string, writeback_position_t> pending_balance_t;
	typedef std::map<std::string, writeback_position_t> labels_t;
	writeback_position_t text, data;
	const writeback_position_t *current;
	pending_balance_t pending_balance;
	labels_t labels;
	codegen();

public:

	codegen(const writeback_position_t &_text,
	        const writeback_position_t &_data) : text(_text), data(_data), current(&text) {}


	void in_text_section()
	{
		current = &text;
	}


	void in_data_section()
	{
		current = &data;
	}


	template <typename T>
	void emit(const T& val)
	{
		current->out->write((char *)&val, sizeof(T));
	}


	template <typename T>
	void emit_n(int32_t count, const T& val)
	{
		for (int32_t x = 0; x < count; x++) {
			current->out->write((char *)&val, sizeof(T));
		}
	}


	void emit_string(const std::string &str)
	{
		current->out->write(str.c_str(), str.length());
	}


	// the current location is noted as a label. go back and write in the right
	// places. memorize for future emissions of this label.
	void label_here(std::string name);

	// if label is already defined, just return address and we are donee
	// if not defined, when the desired label is found, the appropriate
	// address will be written at location fileloc into ostream file.
	void emit_label_address(std::string name);

	// When the file is finished, use this method to see if all labels
	// are "balanced". If not, you should indicate an error and exit from the app
	// due to a label used but not defined.
	inline bool balanced_labels()
	{
		return pending_balance.empty();
	}
};

#endif /* _CODEGEN_H_ */