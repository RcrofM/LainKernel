#include "string.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* ret = dest;
    while (n && (*dest++ = *src++))
        n--;
    while (n--)
        *dest++ = '\0';
    return ret;
}

char* strcat(char* dest, const char* src) {
    char* ret = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++));
    return ret;
}

void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = ptr;
    while (num--)
        *p++ = (unsigned char)value;
    return ptr;
}

void* memcpy(void* dest, const void* src, size_t num) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (num--)
        *d++ = *s++;
    return dest;
}

void* memmove(void* dest, const void* src, size_t num) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    
    if (d < s) {
        while (num--)
            *d++ = *s++;
    } else {
        d += num;
        s += num;
        while (num--)
            *--d = *--s;
    }
    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const unsigned char* p1 = ptr1;
    const unsigned char* p2 = ptr2;
    
    while (num--) {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    while (*str == ' ')
        str++;
    
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

char* itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;
    
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);
    
    if (tmp_value < 0)
        *ptr++ = '-';
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    
    return str;
}

char* utoa(unsigned int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    unsigned int tmp_value;
    
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
    } while (value);
    
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    
    return str;
}

