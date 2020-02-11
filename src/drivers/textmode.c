#include <system/display/textmode.h>

#include <system/chelpers.h>

#include <system/devices/processor.h>

static display_t display = {0};

static u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static u16 vga_entry(unsigned char uc, u8 color) {
	return (u16) uc | (u16) color << 8;
}

static void scroll() {
    memmove(VGA_ADDR, VGA_ADDR + (80 * 2), (80 * 20 * 2));
}

u32 get_cursor_offset() {
	outportb(SCREEN_CTRL, 14);
	int offset = inportb(SCREEN_DATA) << 8;
	outportb(SCREEN_CTRL, 15);
	offset += inportb(SCREEN_DATA);
	return offset * 2;
}

void set_cursor_offset(int offset) {
	offset /= 2;

	outportb(SCREEN_CTRL, 14);
	outportb(SCREEN_DATA, (unsigned char)(offset >> 8));
	outportb(SCREEN_CTRL, 15);
	outportb(SCREEN_DATA, (unsigned char)(offset & 0xFF));
}

u32 get_current_offset(int col, int row) {
	return (row * TEXTMODE_HEIGHT + col);
}

u32 get_offset_row(int offset) {
	return offset / (2 * TEXTMODE_HEIGHT);
}

u32 get_offset_col(int offset) {
	return (offset - (get_offset_row(offset) * 2 * TEXTMODE_HEIGHT)) / 2;
}

void textmode_setup() {
    display.color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    display.buffer = (unsigned char*)VGA_ADDR;

    set_cursor_offset(0);

    for (u32 y = 0; y < TEXTMODE_HEIGHT; y++) {
        for (u32 x = 0; x < TEXTMODE_WIDTH; x++) {
            const u32 index = y * TEXTMODE_WIDTH + x;

            display.buffer[index] = vga_entry(' ', display.color);
        }
    }

    display.x = 0;
    display.y = 0;
}

void textmode_puts(const char* s) {
    for (int i = 0; i < strlen(s); i++) {
        textmode_putc(s[i]);
    }
}

void update_cursor() {
    u16 pos = display.y * TEXTMODE_WIDTH + display.x;

    outportb(SCREEN_CTRL, 14);
	outportb(SCREEN_DATA, pos >> 8);
	outportb(SCREEN_CTRL, 15);
	outportb(SCREEN_DATA, pos);
}

void textmode_putc(unsigned char c) {
    if (!c)
        return;

    if (display.x >= TEXTMODE_WIDTH || c == '\n') {
        display.x = -1;
        display.y++;
    } else if (c == 0x08 && display.x) {
        display.x--;
    } else if (c == 0x09) {
        display.x = (display.x + 8) & ~(7);
    } else if (c == '\r') {
        display.x = 0;
    } else if (c >= ' ') {
        const u32 index = display.y * TEXTMODE_WIDTH + display.x;

        display.buffer[index] = (u16*)vga_entry(c, display.color);
    }

    display.x++;
    //display.y++;

    update_cursor();
}