#ifndef SECURITY_H
#define SECURITY_H

#include "kernel.h"

// Stack canary
#define STACK_CANARY_VALUE 0xDEADBEEF

// Security functions
void security_init(void);
u32 security_get_canary(void);
bool security_check_canary(u32 canary);
bool security_validate_privilege(u8 required_ring);
void security_sanitize_string(char* str, u32 max_len);

// Input validation
bool security_is_printable(char c);
bool security_validate_buffer(const void* buf, u32 size);

#endif // SECURITY_H
