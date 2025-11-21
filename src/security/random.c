#include "random.h"`n#include <kernel/kernel.h>
#include "kernel.h"

// Xorshift128+ state
static u64 random_state[2];

static u64 rdtsc(void) {
    u32 low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return ((u64)high << 32) | low;
}

void random_init(void) {
    // Gather entropy from multiple sources
    u64 entropy1 = rdtsc();
    
    // PIT timer
    u32 pit = inb(0x40);
    pit |= inb(0x40) << 8;
    
    // System control port
    u32 sys = inb(0x61);
    
    // Combine entropy sources
    random_state[0] = entropy1 ^ ((u64)pit << 32);
    random_state[1] = rdtsc() ^ ((u64)sys << 48);
    
    // Ensure non-zero state
    if (random_state[0] == 0) random_state[0] = 1;
    if (random_state[1] == 0) random_state[1] = 1;
    
    // Warm up the generator
    for (int i = 0; i < 100; i++) {
        random_get();
    }
}

u32 random_get(void) {
    // Xorshift128+ algorithm
    u64 s1 = random_state[0];
    u64 s0 = random_state[1];
    
    random_state[0] = s0;
    s1 ^= s1 << 23;
    random_state[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    
    u64 result = random_state[1] + s0;
    
    // Mix in fresh entropy periodically
    static u32 counter = 0;
    if (++counter % 1000 == 0) {
        random_state[0] ^= rdtsc();
    }
    
    return (u32)(result >> 32) ^ (u32)result;
}

void random_get_bytes(u8* buffer, u32 size) {
    for (u32 i = 0; i < size; i += 4) {
        u32 random = random_get();
        u32 remaining = size - i;
        u32 to_copy = remaining < 4 ? remaining : 4;
        
        for (u32 j = 0; j < to_copy; j++) {
            buffer[i + j] = (random >> (j * 8)) & 0xFF;
        }
    }
}

