#include <stdint.h>
#include "helpers.h"


int register_sanity_check(uint16_t r)
{
    if( r <= 7)
        return 1;
    return 0;
}

uint16_t sign_extended(uint16_t x, int bit_count)
{
    if((x >> (bit_count -1)) & 1) /* If the MSG is 1 wich mean that this is a negative number */
    {
        x |= (0xffff - bit_count);
    }

    return x;
}