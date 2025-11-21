#ifndef RANDOM_H
#define RANDOM_H

#include "kernel.h"

// Random number generator functions
void random_init(void);
u32 random_get(void);
void random_get_bytes(u8* buffer, u32 size);

#endif // RANDOM_H
