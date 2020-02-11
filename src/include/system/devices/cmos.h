#pragma once

#include <runtime.h>

#include <system/devices/processor.h>

#define from_bcd(val)  ((val / 16) * 10 + (val & 0xf))

#define	CMOS_ADDRESS     0x70
#define	CMOS_DATA        0x71

enum
{
	CMOS_SECOND = 0,
	CMOS_MINUTE = 2,
	CMOS_HOUR = 4,
	CMOS_DAY = 7,
	CMOS_MONTH = 8,
	CMOS_YEAR = 9
};

struct timeval {
	u32 tv_sec;
	u32 tv_usec;
};

u32 read_cmos();
u32 now();