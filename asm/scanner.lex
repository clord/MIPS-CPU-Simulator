%option noyywrap
%option nounput
%option yylineno
%{ 
#include "sim/types.h"
#include <iostream>     
#include <stdlib.h>
#include <limits.h>
#include "rasm.tab.hpp"

using namespace std;

const int32_t MAX_STR_CONST = 0x9000;
char string_buf[MAX_STR_CONST];
char *string_buf_ptr;
extern int32_t yylineno;

%}

digit    [0-9]
hexdigit [0-9a-fA-F]
alpha    [a-zA-Z]
id       [a-zA-Z][a-zA-Z0-9_]*
ws       [ \t]+

%x string

%%

[ \t\r\n]*"#".*  /*ignore comment lines*/

{ws}  /* ignore whitespace */

[ \t\r\n]+ {
 // found a newline. should terminate current context, etc. 
 // collapses multiple blank lines to a single newline.
 return NEWLINE;
}


\042 {
  string_buf_ptr = string_buf; 
  BEGIN(string);
}
<string>\042 { 
  BEGIN(INITIAL);
  *string_buf_ptr = '\0';
  int32_t length = string_buf_ptr - string_buf; // null deliberately left out; this is asm!
  yylval.sysstring.ptr = string_buf;
  yylval.sysstring.len = length;
  return STRING;
}
<string>\n {
     cerr << "unterminated string constant; line " << yylineno << endl;
     exit(10);
}
<string>\\[0-7]{1,3} {
    /* octal escape sequence */
    int32_t result;
    sscanf(yytext + 1, "%o", &result);
    if (result > 0xff) {
      /* error, constant is out-of-bounds */
       cerr << "octal escape sequence is too large; line " << yylineno << endl;
       exit(10);
    }
    *string_buf_ptr++ = result;
}
<string>\\[0-9]+ {
   cerr << "bad escape sequence; line " << yylineno << endl;
   exit(10);
}
<string>\\n       {*string_buf_ptr++ = '\n';}
<string>\\t       {*string_buf_ptr++ = '\t';}
<string>\\r       {*string_buf_ptr++ = '\r';}
<string>\\b       {*string_buf_ptr++ = '\b';}
<string>\\f       {*string_buf_ptr++ = '\f';}
<string>\\(.|\n)  {*string_buf_ptr++ = yytext[1];}
<string>[^\\\n\042]+ {
  char *yptr = (char*)yytext;
  while (*yptr) {
   *string_buf_ptr++ = *yptr++;
  }
}

".text"   {return TEXT_SECTION;}
".data"   {return DATA_SECTION;}
".word"   {return WORD; }
".byte"   {return BYTE; }
".space"  {return SPACE;}
".ascii"  {return ASCII;}
".asciiz"  {return ASCIIZ;}
\.[a-zA-Z_0-9]*  {return SECTION_IDENT; /*for error reporting*/}


"addi"     {return ADDI;      }
"add"      {return ADD;       }
"subi"     {return SUBI;      }
"b"        {return BRANCH;    }
"beqz"     {return BRANCHEQZ; }
"bge"      {return BRANCHGE;  }
"bne"      {return BRANCHNE;  }
"la"       {return LOADADDR;  }
"lb"       {return LOADBYTE;  }
"li"       {return LOADIMMED; }
"syscall"  {return SYSCALL;   }
"nop"      {return NOOP;      }


[,()] { return *yytext; /* punctuation marks */}

{id}\: {
  // Found a label decl.
  int32_t sl = strlen(yytext);
  yytext[sl-1] = 0; // take out colon
  yylval.sysstring.len = sl;
  yylval.sysstring.ptr = yytext; 
  return LABELDECL;
}

{id} {
  // Found a label ref. 
  int32_t sl = strlen(yytext);
  yylval.sysstring.len = sl;
  yylval.sysstring.ptr = yytext;
  return LABELREF;
}

\${digit}+ {
  // found a register reference
  yylval.sysword = strtol(yytext+1, NULL, 10);
  if (yylval.sysword < 0 || yylval.sysword > 31) {
    cerr << "invalid register \'"<< yytext << "\'; line " << yylineno << endl;
    exit(10);
  }
  return REGISTER;
}

\-?{digit}+ {
  // found an integer
  yylval.sysword = strtol(yytext, NULL, 10);
  return INTEGER;
}

0x{hexdigit}+ {
  // found an integer
  yylval.sysword = strtol(yytext, NULL, 16);
  return INTEGER;
}

\$[-a-zA-Z0-9_]* {
  return INVALID_REGISTER;
}

. {
  cerr << "invalid character; line " << yylineno << endl;
  exit(10);
}

%%
