#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <system/devices/display.h>
#include <system/devices/processor.h>

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

#define panic(...) {kprintf("*** KERNEL PANIC ***\nSource File: %s", __FILE__); kprintf("\nSource Line: %d\n", __LINE__); kprintf("Source Function: %s", __func__); for(;;);}

#define __packed __attribute__((packed))

#define __aligned(__align) __attribute__((aligned(__align)))

#define __unused(__stuff) (void)(__stuff)

#define __create(__type) ((__type *)malloc(sizeof(__type)))