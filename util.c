#include "gba.h"
#include "util.h"

void memcpy(char *dst, const char *src, unsigned int size) {
    while (size--) {
        *dst++ = *src++;
    }
}

void memset(char *dst, char val, unsigned int size) {
    while (size--) *dst++ = val;
}

void fmt_u32(u32 value, char* buf) {
    value = value % 1000000000; // 10^9 (9 digits max)
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    int i = 0;
    // Find the digits in reverse order.
    while (value > 0) {
        buf[i] = '0' + (value % 10);
        value /= 10;
        i++;
    }

    // Null-terminate the string
    buf[i] = '\0';

    // Reverse the string to get the correct order
    int start = 0;
    int end = i - 1;
    while (start < end) {
        // Swap the characters
        char temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
        start++;
        end--;
    }
}
