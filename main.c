#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "includes/helpers.h"
#include "includes/memory.h"
#include "includes/constants.h"
#include "includes/log.h"

/*
There is 3 types of registers in the LC-3:
    - 8 generale puprose registers ( from R0 to R7)
    - instrruction pointer register ( PC )
    - condtions register ( COND )
There are all 16 bits length
There is 10 registers
*/

enum REG 
{
    R_REG0=0,
    R_REG1,
    R_REG2,
    R_REG3,
    R_REG4,
    R_REG5,
    R_REG6,
    R_REG7,
    R_COND,
    R_PC
};

uint16_t registers[10];

/* Conditions flags */
enum FLAGS 
{
    FL_POS = 1 << 0,
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2,
};

/*instrructions opcode */
/*
ADD,AND,NOT are operate 
LD, LDI, LDR, LEA, ST, STR, STI are for data movement
BR, JSR/JSRR, JMP, RTI, TRAP are for control
*/

enum OPCODE
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load regis ter */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

void update_flags(uint16_t r) {
    if(registers[r] == 0)
    {
        registers[R_COND] = FL_ZRO;
    } else if (registers[r] >> 15) 
    {
        registers[R_COND] = FL_NEG;
    } else 
    {
        registers[R_COND] = FL_POS;
    }
    
}


/*
    
*/
int instruction_sti(uint16_t instr)
{
    uint16_t sr = GET_DR(instr); /* The Sr field is after the opcode 
                                    so we can obtain its value with 
                                    the same way for Dr */

    uint16_t pc_offset = sign_extended(GET_PCOFFSET9(instr), 9);
    uint16_t address = registers[R_PC] + pc_offset
    write_mem(
        read_mem(address),
        registers[sr]
    );

    return 0;
}


/*
    ST SR, LABEL
*/
int instruction_st(uint16_t instr)
{
    uint16_t sr = GET_DR(instr); /* The Sr field is after the opcode 
                                    so we can obtain its value with 
                                    the same way for Dr */

    uint16_t pc_offset = sign_extended(GET_PCOFFSET9(isntr), 9);

    write_mem(registers[R_PC] + pc_offset, registers[sr]);

    return 0;
}


/*
    NOT DR, SR
*/
int instruction_not(uint16_t instr)
{
    uint16_t dr = GET_DR(instr);
    
    REG_CHECK(dr);

    uint16_t op1 = GET_OP1(instr);

    registers[dr] = ~registers[op1];

    update_flags(dr);

    return 0;
}


/*
    LEA DR, LABEL
*/
int instruction_lea(uint16_t instr)
{
    uint16_t dr = GET_DR(instr);

    REG_CHECK(dr);

    uint16_t pc_offset = sign_extended(GET_PCOFFSET9(instr), 9);

    registers[dr] = registers[R_PC] + pc_offset;

    update_flags(dr);

    return 0;       

}


/* 
    LDR DR, BaseR, offset6
*/
int instruction_ldr(uint16_t instr) {

    uint16_t dr = GET_DR(instr);

    REG_CHECK(dr);

    uint16_t op1 = GET_OP1(instr);

    uint16_t pc_offset = sign_extended(GET_PCOFFSET6(instr), 6);
    
    registers[dr] = read_mem(registers[op1] + pc_offset);

    update_flags(dr);

    return 0;

}


/*
    LD DR, LABEL
*/
int instruction_ld(uint16_t instr) {
    uint16_t dr = GET_DR(instr);

    REG_CHECK(dr);

    uint16_t pc_offset = sign_extended(GET_PCOFFSET9(instr), 9);

    registers[dr] = read_mem(registers[R_PC] + pc_offset);

    return 0;
}


/* 
    JSR LABEL
    JSRR BaseR
*/
int instruction_jsr(uint16_t instr) {

    registers[R_REG7] = registers[R_PC]++;

    if(IS_RELATIVE11(instr))
    {   
        /* JSR */
        uint16_t pc_offset = sign_extended(GET_PCOFFSET11(instr), 9);
        registers[R_PC] =+ pc_offset;
     
    } else {

        /* JSSR */
        uint16_t reg = GET_OP1(instr);
 
        REG_CHECK(reg);

        registers[R_PC] = registers[reg];

    }

    return 0;
}
/* 
Also handle RET ( RET <=> JMP R7)
    JMP BaseR
    RET
*/
int instruction_jmp(uint16_t instr) {
    registers[R_PC] = registers[GET_OP1(instr)];

    return 0;
}


/* 
    AND DR, SR1, SR2
    AND DR, SR1, imm5
*/
int instruction_and(uint16_t instr) {
    uint16_t dr = GET_DR(instr);
      
    REG_CHECK(dr);
    
    uint16_t op1 = GET_OP1(instr);

    REG_CHECK(op1);

    uint16_t imm_mode = GET_IMM_MODE5(instr);

    if(imm_mode)
    {
        uint16_t imm5 = sign_extended(GET_IMM5(instr), 5);
        registers[dr] = registers[op1] & imm5;
    } else {
        uint16_t op2 = GET_OP2(instr);
        registers[dr] = registers[op1] & registers[op2];
    }

    update_flags(dr);

    return 0;
}


/*
    LDI DR, LABEL
*/
int instruction_ldi(uint16_t instr) {
    uint16_t dr = GET_DR(instr);
   
    REG_CHECK(dr);

    uint16_t pc_offset = sign_extended(instr & 0x1ff, 9);

    registers[R_REG0] = read_mem(read_mem(registers[R_PC] + pc_offset));

    update_flags(dr);

    return 0;
}


/*
    BRn LABEL BRzp LABEL
    BRz LABEL BRnp LABEL
    BRp LABEL BRnz LABEL
    BR† LABEL BRnzp LABEL
*/
int instruction_br(uint16_t instr) {
    if(IS_BR_N_SET(instr) | IS_BR_P_SET(instr) | IS_BR_Z_SET(instr))
    {
        uint16_t pc_offset = sign_extended(GET_PCOFFSET9(instr), 9);
        registers[R_PC] += pc_offset;

        return 0;

    } else {
        return -EINVAL;
    }
}


/* 
    ADD DR, SR1, SR2
    ADD DR, SR1, imm5
*/
int instruction_add(uint16_t instr) {
    uint16_t dr = GET_DR(instr);

    REG_CHECK(dr);

    uint16_t op1 = GET_OP1(instr);

    REG_CHECK(op1);


    uint16_t imm_mode = GET_IMM_MODE5(instr);

    if(imm_mode)
    {
        uint16_t imm5 = sign_extended(GET_IMM5(instr), 5);
        registers[dr] = registers[op1] + imm5;
    } else {
        uint16_t op2 = GET_OP2(instr);
        
        REG_CHECK(op2);
        
        registers[dr] = registers[op1] + registers[op2];
    }

    update_flags(dr);

    return 0;
}


int main() {
    /* One conditions flag should be set at any given time */ 
    registers[R_COND] = FL_ZRO;

    /*Set the programm counter at the start */
    registers[R_PC] = PROG_ADDR_START;

    int state = 1;
    int operation_result = 1;
    
    do {
        
        /* Fetch the insutrction to execute */
        uint16_t instruction = read_mem(registers[R_PC]++);
        uint16_t opcode = GET_OPCODE(instruction);

        /* opcode handler */
        switch(opcode) 
        {
            case OP_BR:
                operation_result = instruction_br(instruction);
                if(operation_result < 0)
                {
                    log_error("{ BR N-Z-P, PCoffsetT9 } : no condition bit set:\n\tPC = %hu\n\terrno = %d\n", registers[R_PC], operation_result);
                    state = 0;
                }

                break;
            case OP_ADD:         
                operation_result = instruction_add(instruction);
                if (operation_result < 0)
                {
                    log_error("{ ADD Dr, Sr1, Sr2|imm5 } :\n\t\tPC = %hu\n\t\terrno = %d\n", registers[R_PC],operation_result);
                    state = 0;
                }

                break;
            case OP_LD:
                operation_result = instruction_ld(instr);

                if(operation_result < 0)
                {
                    log_error(" { LD DR, LABEL } :\n\t\tPC = %hu\n\t\terrno = %d\n", registers[R_PC], operation_result);
                }
                
                break;
            case OP_ST:

                operation_result = instruction_st(instr);

                if( operation_result < 0)
                {
                    log_error("{ ST SR, PCoffset9 }:\n\tPC = %hu\n\terrno = %d\n", registers[R_PC], operation_result);
                    state = 0;
                }

                break;
            case OP_JSR:
                operation_result = instruction_jsr(instruction);
                if(operation_result < 0)
                {
                    log_error("{ JSR|JSSR PCoffset11|BaseR }:\n\tPC = %hu\n\terrno = %d\n", registers[R_PC], operation_result);
                    state = 0;
                }

                break;
            case OP_AND:

                operation_result = instruction_and(instruction);

                if(operation_result < 0)
                {
                    log_error("{ AND dst op1, op2 } :\n\t\tPC = %hu\n\t\terrno = %d\n", registers[R_PC],operation_result);
                    state = 0;
                }
                break;
            case OP_LDR:
                operation_result = instruction_ldr(instruction);

                if(operation_result < 0)
                {
                    log_error("{ LDR Dr, BaseR, offset6 } :\n\t\tPC = %hu\n\t\terrno = %d\n", registers[R_PC],operation_result);
                    state = 0;
                }
                break;
            case OP_STR:

                operation_result = instruction_str(instrruction);

                if(operation_result < 0)
                {
                    log_error("{ STR SR, BaseR, offset6 } :\n\t\tPC = %hu\n\t\terrno = %d\n",registers[R_PC], operation_result);
                }

                break;
            case OP_RTI:

                log_error("RTi instructions not implemented ( PC = %hu )\n");
                
                state = 0;

                break;
            case OP_NOT:
                operation_result = instruction_not(instr);

                if(operation_result < 0)
                {
                    log_error("{ NOT DR, SR } :\n\t\tPC = %hu\n\t\terrno = %d\n",registers[R_PC], operation_result);
                }
                break;
            case OP_LDI:

                operation_result = instruction_ldi(instruction);

                if(operation_result < 0)
                {
                    log_error("{ LDI Dr, PCoffset9 } :\n\t\tPC = %hu\n\t\terrno = %d\n", registers[R_PC],operation_result);
                    state = 0;
                }
                break;
            case OP_STI:
                
                operation_result = instruction_sti(instr);

                if(operation_result < 0)
                {
                    log_error("{ STI SR, LABEL } :\n\t\tPC = %hu\n\t\terrno = %d\n", registers[R_PC], operation_result);
                }

                break;
            case OP_JMP:
            
                /* also handle RET instruction */
                instruction_jmp(instruction);
                
                break;
            case OP_RES:
                
                log_error("RES instructions not implemented ( PC = %hu )\n");
                
                state = 0;

                break;
            case OP_LEA:
                operation_result = instruction_lea(instruction);

                if(operation_result < 0)
                {
                    log_error("{ LEA Dr, PCoffset9 } :\n\t\tPC = %hu\n\t\terrno = %d\n", registers[R_PC],operation_result);
                    state = 0;
                }

                break;
            case OP_TRAP:
                break;
        }


    } while(state);

    return 0;
}