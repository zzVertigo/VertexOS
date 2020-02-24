#include <system/drivers/timer.h>

u32 timer_ticks = 0;
u32 timer_subticks = 0;

u32 timer_drift = 0;
u32 _timer_drift = 0;

static u32 behind = 0;

void timer_phase(int hz) {
    int divisor = PIT_SCALE / hz;

    outportb(PIT_CONTROL, PIT_SET);
    outportb(PIT_A, divisor & PIT_MASK);
    outportb(PIT_A, (divisor >> 8) & PIT_MASK);
}

void timer_handler(registers_t *regs) {
    if (++timer_subticks == SUBTICKS_PER_TICK || (behind && ++timer_subticks == SUBTICKS_PER_TICK)) {
        timer_ticks++;
        timer_subticks = 0;

        if (timer_ticks % RESYNC_TIME == 0) {
            // u32 new_time = read_cmos();
            // _timer_drift = new_time - boot_time - timer_ticks;

            // if (_timer_drift > 0)
            //     behind = 1;
            // else
            //     behind = 0;

            behind = 0;
        }       
    }
}

void timer_setup() {
    register_interrupt_handler(IRQ0, &timer_handler);

    timer_phase(SUBTICKS_PER_TICK);
}
