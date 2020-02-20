#include <drivers/textmode.h>

#include <system/chelpers.h>

#include <system/devices/processor.h>

static display_t display = {0};

int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
int get_cursor_offset();

void set_cursor_offset(int offset);

void puts_at(char* message, int x, int y);

static u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return (bg << 4) | (fg & 0x0F);
}
 
static u16 vga_entry(unsigned char uc, u8 color) {
	return (u16) uc | (u16) color << 8;
}

void textmode_setup() {
    display.color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    display.buffer = (u16*)0xB8000;

    for (int i = 0; i < (MAX_COLS * MAX_ROWS); i++) {
        display.buffer[i] = vga_entry(' ', display.color);
    }

    display.x = 0;
    display.y = 0;

    set_cursor_offset(get_offset(display.x, display.y));
}

void clear_screen() {
    for (int i = 0; i < (MAX_COLS * MAX_ROWS); i++) {
        display.buffer[i] = vga_entry(' ', display.color);
    }  

        display.x = 0;
    display.y = 0;

    set_cursor_offset(get_offset(display.x, display.y));
}

void puts(char* message) {
    puts_at(message, -1, -1);
}

void puts_at(char* message, int col, int row) {
    int offset;

    if (col >= 0 && row >= 0) {
        offset = get_offset(col, row);
    } else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    int i = 0;

    while (message[i] != 0) {
        offset = putc(message[i++], col, row);

        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

u32 putc(char c, int col, int row) {
    int offset;

    u16 *location;

    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else
        offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row + 1);
    } else if (c == 0x08) {
        display.buffer[offset / 2] = vga_entry(' ', display.color);

        offset -= 2;
    } else {
        display.buffer[offset / 2] = vga_entry(c, display.color);

        offset += 2;
    }

    if (offset >= MAX_ROWS * MAX_COLS * 2) {
        int i;

        for (i = 1; i < MAX_ROWS; i++) {
            memcpy((u16*)(get_offset(0, i) + 0xB8000), (u16*)(get_offset(0, i-1) + 0xB8000), MAX_COLS * 2);

            char *lastline = (char*)(get_offset(0, MAX_ROWS - 1) + (u16*)0xB8000);

            for (i = 0; i < MAX_COLS * 2; i++) {
                lastline[i] = 0;
            }

            offset -= 2 * MAX_COLS;
        }
    }

    set_cursor_offset(offset);

    return offset;
}

void set_cursor_offset(int offset) {
    offset /= 2;

    outportb(SCREEN_CTRL, 14);
    outportb(SCREEN_DATA, (u8)(offset >> 8));
    outportb(SCREEN_CTRL, 15);
    outportb(SCREEN_DATA, (u8)(offset & 0xFF));
}

int get_cursor_offset() {
    outportb(SCREEN_CTRL, 14);
    int offset = inportb(SCREEN_DATA) << 8;
    outportb(SCREEN_CTRL, 15);

    offset += inportb(SCREEN_DATA);

    return offset * 2;
}

// not it
int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

// not it
int get_offset_row(int offset) {
    return offset / (2 * MAX_COLS);
}

// not it
int get_offset_col(int offset) {
    return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}