#pragma once

#include <runtime.h>

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

#define VGA_ADDR 0xB8000
#define MAX_COLS 80
#define MAX_ROWS 25

#define SCREEN_DATA 0x3D5
#define SCREEN_CTRL 0x3D4

typedef struct {
    size_t x, y;
    u8 color;
    u16* buffer;
} display_t;

void textmode_setup();
void puts(char* message);
u32 putc(char c, int x, int y);