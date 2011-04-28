#ifndef _SYSCALL_H_
#define _SYSCALL_H_

const int ZERO_REG = 0;
const int AT_REG   = 1;
const int V_REG    = 2;
const int A_REG    = 4;
const int LT_REG   = 8;
const int S_REG    = 16;
const int HT_REG   = 24;
const int K_REG    = 26;
const int GP_REG   = 28;
const int SP_REG   = 29;
const int FP_REG   = 30;
const int RA_REG   = 31;

class cpu_core;

void sysc_op(cpu_core * cpu);

#endif /* _SYSCALL_H_ */
