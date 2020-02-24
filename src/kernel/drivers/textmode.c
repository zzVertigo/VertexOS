#include <system/drivers/textmode.h>

static display_t display;

u8 command_buffer[128];
u32 command_index = 0;

extern const char *username;
extern const char *hostname;

static u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
  	return (bg << 4) | (fg & 0x0F);
}
 
static u16 vga_entry(char c, u8 color) {
	return c | (color << 8);
}

static void move_cursor() {
    u16 cursor_location = display.cursor_y * 80 + display.cursor_x;

    outportb(0x3D4, 14);
    outportb(0x3D5, cursor_location >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, cursor_location);
}

void scroll_screen() {
    if (display.cursor_y >= 25) {
        int i;

        for (i = 0; i < (MAX_ROWS - 1) * MAX_COLS; i++) {
            display.buffer[i] = display.buffer[i + 80];
        }

        for (i = (MAX_ROWS - 1) * MAX_COLS; i < (MAX_ROWS * MAX_COLS); i++)
            display.buffer[i] = vga_entry(' ', display.color);

        display.cursor_y = 24;
    }
}

void clear_screen() {
    int i;

    for (int i = 0; i < (MAX_COLS * MAX_ROWS); i++)
        display.buffer[i] = vga_entry(' ', display.color);

    display.cursor_x = 0;
    display.cursor_y = 0;

    move_cursor();
}

void textmode_setup() {
    display.color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    display.buffer = (u16*)0xB8000;

    display.cursor_x = 0;
    display.cursor_y = 0;
    
    clear_screen();
    move_cursor();
}

void puts(char* message) {
    int i = 0;

    while(message[i] != 0) {
        putc(message[i++]);
    } 
}

void keyboard_add_press(u8 c) {
    command_buffer[command_index++] = c;
}

void keyboard_enter() {
}

void putc(char c) {
    int position = (display.cursor_y * MAX_COLS + display.cursor_x);

    if (c == 0x08 && display.cursor_x) { // backspace
        if (display.cursor_x > 18) {
            display.buffer[position - 1] = vga_entry(' ', display.color);
            display.cursor_x--;
        }
    } else if (c == 0x09) // tab
        display.cursor_x = (display.cursor_x + 8) & ~(7);
    else if (c == '\r')
        display.cursor_x = 0;
    else if (c == '\n') {
        display.cursor_x = 0;
        display.cursor_y++;
    } else if (c >= ' ') {
        display.buffer[position] = vga_entry(c, display.color);
        display.cursor_x++;
    }

    if (display.cursor_x >= MAX_COLS) {
        display.cursor_x = 0;
        display.cursor_y++;
    } 

    scroll_screen();
    move_cursor();
}