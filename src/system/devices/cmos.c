#include <system/devices/cmos.h>

u32 boot_time = 0;

void cmos_dump(u16 *values) {
    u16 index;

    for (index = 0; index < 128; index++) {
        outportb(CMOS_ADDRESS, index);

        values[index] = inportb(CMOS_DATA);
    }
}

int is_update_in_progress(void) {
    outportb(CMOS_ADDRESS, 0x0a);

    return inportb(CMOS_DATA) & 0x80;
}

void get_date(u16 *month, u16 *day) {
    u16 values[128];
    cmos_dump(values);

    *month = from_bcd(values[CMOS_MONTH]);
    *day = from_bcd(values[CMOS_DAY]);
}

void get_time(u16 *hours, u16 *minutes, u16 *seconds) {
    u16 values[128];
    cmos_dump(values);

    *hours = from_bcd(values[CMOS_HOUR]);
    *minutes = from_bcd(values[CMOS_MINUTE]);
    *seconds = from_bcd(values[CMOS_SECOND]);
}

u32 secs_of_years(int years) {
    u32 days = 0;
    years += 2000;

    while (years > 1969) {
        days += 365;
        if (years % 4 == 0) {
            if (years % 100 == 0) {
                if (years % 400 == 0) {
                    days++;
                }
            } else {
                days++;
            }
        }

        years--;
    }

    return days * 86400;
}

u32 secs_of_month(int months, int year) {
	year += 2000;

	u32 days = 0;
	switch(months) {
		case 11:
			days += 30;
		case 10:
			days += 31;
		case 9:
			days += 30;
		case 8:
			days += 31;
		case 7:
			days += 31;
		case 6:
			days += 30;
		case 5:
			days += 31;
		case 4:
			days += 30;
		case 3:
			days += 31;
		case 2:
			days += 28;
			if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0))) {
				days++;
			}
		case 1:
			days += 31;
		default:
			break;
	}
	return days * 86400;
}

u32 read_cmos() {
    u16 values[128];
    u16 old_values[128];

    while(is_update_in_progress())
        ;

    cmos_dump(values);

    do {
        memcpy(old_values, values, 128);

        while(is_update_in_progress())
            ;

        cmos_dump(values);
    } while ((old_values[CMOS_SECOND] != values[CMOS_SECOND]) ||
		 (old_values[CMOS_MINUTE] != values[CMOS_MINUTE]) ||
		 (old_values[CMOS_HOUR] != values[CMOS_HOUR])     ||
		 (old_values[CMOS_DAY] != values[CMOS_DAY])       ||
		 (old_values[CMOS_MONTH] != values[CMOS_MONTH])   ||
		 (old_values[CMOS_YEAR] != values[CMOS_YEAR]));

    u32 time =
        secs_of_years(from_bcd(values[CMOS_YEAR] - 1) +
        secs_of_month(from_bcd(values[CMOS_MONTH] - 1), from_bcd(values[CMOS_YEAR])) +
        (from_bcd(values[CMOS_DAY]) - 1) * 86400 +
	    (from_bcd(values[CMOS_HOUR])) * 3600 +
	    (from_bcd(values[CMOS_MINUTE])) * 60 +
	    from_bcd(values[CMOS_SECOND]) + 0);

    return time;
}

int gettimeofday(struct timeval *t, void *z) {
    t->tv_sec = boot_time + timer_ticks + timer_drift;
    t->tv_usec = timer_subticks * 1000;

    return 0;
}

u32 now() {
    struct timeval t;
    gettimeofday(&t, NULL);

    return t.tv_sec;
}