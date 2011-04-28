#ifndef _MTYPES_H_
#define _MTYPES_H_

typedef unsigned char byte;
typedef unsigned int system_word;
typedef struct _system_string {
  int len;
  char * ptr;
} system_string;
typedef struct _sys_offset {
  unsigned int reg;
  int offset; 
} sys_offset;
typedef union YYSTYPE {
    system_string sysstring;
    system_word   sysword;
    sys_offset sysoffset;
    
} YYSTYPE;
#define YYSTYPE_IS_DECLARED 1
#endif /* MTYPES */

