#ifndef _MTYPES_H_
#define _MTYPES_H_
#include <stdint.h>

typedef uint8_t byte;

struct system_string {
	int32_t len;
	char *ptr;
};

struct sys_offset {
	uint32_t reg;
	int32_t offset;
};

union YYSTYPE {
	system_string sysstring;
	uint32_t sysword;
	sys_offset sysoffset;
};

#define YYSTYPE_IS_DECLARED 1
#endif /* MTYPES */