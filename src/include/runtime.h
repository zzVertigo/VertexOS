#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <system/drivers/display.h>
#include <system/drivers/processor.h>
#include <system/drivers/textmode.h>

#define NULL ((void *)0)

typedef void s0;
typedef char s8;
typedef short s16;
typedef long s32;
typedef long long s64;

typedef void u0;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned char uchar;
typedef unsigned short wchar;
typedef unsigned short ushort;
typedef unsigned int uint;

extern u32 boot_time;
extern u32 timer_ticks;
extern u32 timer_subticks;
extern u32 timer_drift;

void __panic(const char *file, u32 line, const char *func, const char *reason);
void __assert(const char *file, u32 line, const char* func, const char *reason);

void reboot();

#define NAME "VertexOS"

#define MAJOR 0
#define MINOR 0
#define BUILD 1

#define COPYRIGHT_OWNER "zzVertigo"
#define COPYRIGHT_YEAR 2020

#define panic(str, ...) __panic(__FILE__, __LINE__, __func__, str);
#define assert(cond) ((cond) ? (void)0 : __assert(__FILE__, __LINE__, __func__, #cond))

#define __noreturn __attribute__((noreturn))

#define __aligned(__align) __attribute__((aligned(__align)))

#define __unused(__stuff) (void)(__stuff)

#define __create(__type) ((__type *)malloc(sizeof(__type)))