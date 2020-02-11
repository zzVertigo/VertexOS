#include <system/chelpers.h>

void *memchr(const void *str, int c, size_t n) {
    unsigned char *s = (unsigned char *)str;

    for (size_t i = 0; i < n; i++) {
        if (*(s + i) == c) {
            return (s + i);
        }
    }

    return NULL;
}

int memcmp(const void *str1, const void *str2, size_t n) {
    const unsigned char *s1 = str1;
    const unsigned char *s2 = str2;

    for (size_t i = 0; i < n; i++) {
        if (*(s1 + i) != *(s2 + i)) {
            return *(s1 + i) - *(s2 + i);
        }
    }

    return 0;
}

void *memmove(void *dest, const void *src, size_t n) {
    size_t i;
    const unsigned char *usrc = src;
    unsigned char *udest = dest;

    if (udest < usrc) {
        for (i = 0; i < n; i++)
            udest[i] = usrc[i];
    } else if (udest > usrc) {
        for (i = n; i > 0; i--)
            udest[i - 1] = usrc[i - 1];
    }

    return dest;
}

void *memcpy(void *s1, const void *s2, size_t n) {
    char *cdest;
    char *csrc;
    unsigned int *ldest = (unsigned int *)s1;
    unsigned int *lsrc = (unsigned int *)s2;

    while (n >= sizeof(unsigned int)) {
        *ldest++ = *lsrc++;
        n -= sizeof(unsigned int);
    }

    cdest = (char *)ldest;
    csrc = (char *)lsrc;

    while (n > 0) {
        *cdest++ = *csrc++;
        n -= 1;
    }

    return s1;
}

void *memset(void *str, int c, size_t n) {
    char *s = str;

    for (size_t i = 0; i < n; i++) {
        *(s + i) = c;
    }

    return str;
}

size_t strlen(const char *str) {
    size_t length = 0;

    while (str[length])
        length++;

    return length;
}

char *strcpy(char *dest, const char *src) {
    do {
        *dest++ = *src++;
    } while (*src != 0);
}

int strcmp(char *str1, char *str2) {
    int i = 0;
    int failed = 0;

    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            failed = 1;
            break;
        }

        i++;
    }

    if ((str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0'))
        failed = 1;

    return failed;
}