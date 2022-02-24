#include <stdint.h>

/* Registers are 16 bits length so there is 2^16 address */
uint16_t memory[UINT16_MAX];

uint16_t write_mem(uint16_t address, uint16_t content)
{
    memory[address] = content;
    return memory[address];
}
uint16_t read_mem(uint16_t address)
{
    return memory[address];
}