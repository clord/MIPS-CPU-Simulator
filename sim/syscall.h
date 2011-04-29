#ifndef _SYSCALL_H_
#define _SYSCALL_H_

const int32_t ZERO_REG = 0;
const int32_t AT_REG = 1;
const int32_t V_REG = 2;
const int32_t A_REG = 4;
const int32_t LT_REG = 8;
const int32_t S_REG = 16;
const int32_t HT_REG = 24;
const int32_t K_REG = 26;
const int32_t GP_REG = 28;
const int32_t SP_REG = 29;
const int32_t FP_REG = 30;
const int32_t RA_REG = 31;

class cpu_core;

void sysc_op(cpu_core *cpu);

#endif /* _SYSCALL_H_ */