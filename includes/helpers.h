#include <stdint.h>
#include "constants.h"

#define PCOFFSET6_MASK  0b1111111111000000
#define PCOFFSET9_MASK  0b1111111000000000
#define PCOFFSET11_MASK 0b1111000000000000

#define GET_OPCODE(instr)       (instr >> 12)
#define GET_DR(instr)           ((instr >> 9) & 0x7)
#define GET_OP1(instr)          ((instr >> 6) & 0x7)
#define GET_OP2(instr)          (instr & 0x7)

#define GET_
#define IS_RELATIVE11(instr)    ((instr >> 11) & 0x1)

#define GET_IMM_MODE5(instr)    ((instr >> 5) & 0x1)
#define GET_IMM5(instr)         (instr & 0x1F)

#define GET_PCOFFSET6(instr)    (instr & PCOFFSET6_MASK)
#define GET_PCOFFSET9(instr)    (instr & PCOFFSET9_MASK)
#define GET_PCOFFSET11(instr)   (instr & PCOFFSET11_MASK)

#define LUL 0x10
#define IS_BR_N_SET(instr)      ((instr >> 9) & (0x1 << 0x2))
#define IS_BR_Z_SET(instr)      ((instr >> 9) & (0x1 << 0x1))
#define IS_BR_P_SET(instr)      ((instr >> 9) & 0x1)

#define REG_CHECK(r) do {\
   if (!register_sanity_check(r)) { return -EINVAL; }\
} while(0)



int register_sanity_check(uint16_t r);
uint16_t sign_extended(uint16_t x, int bit_count);
