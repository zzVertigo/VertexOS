#include <system/devices/keyboard.h>
#include <cpu/isr.h>
#include <drivers/textmode.h>

static void keyboard_callback();

// Scancode -> ASCII
const u8 lower_ascii_codes[256] = {
    0x00,  ESC,  '1',  '2',     /* 0x00 */
     '3',  '4',  '5',  '6',     /* 0x04 */
     '7',  '8',  '9',  '0',     /* 0x08 */
     '-',  '=',   BS, '\t',     /* 0x0C */
     'q',  'w',  'e',  'r',     /* 0x10 */
     't',  'y',  'u',  'i',     /* 0x14 */
     'o',  'p',  '[',  ']',     /* 0x18 */
    '\n', 0x00,  'a',  's',     /* 0x1C */
     'd',  'f',  'g',  'h',     /* 0x20 */
     'j',  'k',  'l',  ';',     /* 0x24 */
    '\'',  '`', 0x00, '\\',     /* 0x28 */
     'z',  'x',  'c',  'v',     /* 0x2C */
     'b',  'n',  'm',  ',',     /* 0x30 */
     '.',  '/', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '-',  '4',     /* 0x48 */
     '5',  '6',  '+',  '1',     /* 0x4C */
     '2',  '3',  '0',  '.',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};

// Scancode -> ASCII
const u8 upper_ascii_codes[256] = {
    0x00,  ESC,  '!',  '@',     /* 0x00 */
     '#',  '$',  '%',  '^',     /* 0x04 */
     '&',  '*',  '(',  ')',     /* 0x08 */
     '_',  '+',   BS, '\t',     /* 0x0C */
     'Q',  'W',  'E',  'R',     /* 0x10 */
     'T',  'Y',  'U',  'I',     /* 0x14 */
     'O',  'P',  '{',  '}',     /* 0x18 */
    '\n', 0x00,  'A',  'S',     /* 0x1C */
     'D',  'F',  'G',  'H',     /* 0x20 */
     'J',  'K',  'L',  ':',     /* 0x24 */
     '"',  '~', 0x00,  '|',     /* 0x28 */
     'Z',  'X',  'C',  'V',     /* 0x2C */
     'B',  'N',  'M',  '<',     /* 0x30 */
     '>',  '?', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '-',  '4',     /* 0x48 */
     '5',  '6',  '+',  '1',     /* 0x4C */
     '2',  '3',  '0',  '.',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};


const uint8_t lower_ascii_codes_dvorak[256] = {
    0x00,  ESC,  '1',  '2',     /* 0x00 */
     '3',  '4',  '5',  '6',     /* 0x04 */
     '7',  '8',  '9',  '0',     /* 0x08 */
     '[',  ']',   BS, '\t',     /* 0x0C */
    '\'',  ',',  '.',  'p',     /* 0x10 */
     'y',  'f',  'g',  'c',     /* 0x14 */
     'r',  'l',  '/',  '=',     /* 0x18 */
    '\n', 0x00,  'a',  'o',     /* 0x1C */
     'e',  'u',  'i',  'd',     /* 0x20 */
     'h',  't',  'n',  's',     /* 0x24 */
     '-',  '`', 0x00, '\\',     /* 0x28 */
     ';',  'q',  'j',  'k',     /* 0x2C */
     'x',  'b',  'm',  'w',     /* 0x30 */
     'v',  'z', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '[',  '4',     /* 0x48 */
     '5',  '6',  '}',  '1',     /* 0x4C */
     '2',  '3',  '0',  'v',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};

// Scancode -> ASCII
const u8 upper_ascii_codes_dvorak[256] = {
    0x00,  ESC,  '!',  '@',     /* 0x00 */
     '#',  '$',  '%',  '^',     /* 0x04 */
     '&',  '*',  '(',  ')',     /* 0x08 */
     '{',  '}',   BS, '\t',     /* 0x0C */
     '"',  '<',  '>',  'P',     /* 0x10 */
     'Y',  'F',  'G',  'C',     /* 0x14 */
     'R',  'L',  '?',  '+',     /* 0x18 */
    '\n', 0x00,  'A',  'O',     /* 0x1C */
     'E',  'U',  'I',  'D',     /* 0x20 */
     'H',  'T',  'N',  'S',     /* 0x24 */
     '_',  '~', 0x00,  '|',     /* 0x28 */
     ':',  'Q',  'J',  'K',     /* 0x2C */
     'X',  'B',  'M',  'W',     /* 0x30 */
     'V',  'Z', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '[',  '4',     /* 0x48 */
    '5',  '6',  '}',  '1',     /* 0x4C */
     '2',  '3',  '0',  'v',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};

u8 shift;
u8 ctrl;
u8 keypresses[256];

u8 kb_buff[128];
u8 kb_buff_hd;
u8 kb_buff_tl;

static void pull_input() {
    u8 next_hd = (kb_buff_hd + 1) % 128;

    if (next_hd == kb_buff_tl)
        return;

    u8 byte = inportb(0x60);

    if (byte == 0)
        return;

    if (byte & 0x80) {
        u8 pressedbyte = byte & 0x7F;

        if (pressedbyte == 0x2A) {
            shift = shift & 0x02;
        } else if (pressedbyte == 0x36) {
            shift = shift & 0x01;
        } else if (pressedbyte == 0x1D) {
            ctrl = 0;
        } else if (pressedbyte == 0x1C) { // enter
            keyboard_enter();
        }

        keypresses[pressedbyte] = 0;

        return;
    }

    if (keypresses[byte] < 10 && keypresses[byte] > 0) {
        keypresses[byte]++;
        return;
    }

    keypresses[byte]++;

    if (byte == 0x2A) {
        shift = shift | 0x01;
        return;
    } else if (byte == 0x36) {
        shift = shift | 0x02;
    } else if (byte == 0x1D) {
        ctrl = 1;
        return;
    }

    const u8 *codes;

    if (ctrl) {
        if (lower_ascii_codes[byte] == 'd') {
            kb_buff[kb_buff_hd] = EOT;
            kb_buff_hd = next_hd;
            return;
        }
    } else if (shift) {
        codes = upper_ascii_codes;
    } else {
        codes = lower_ascii_codes;
    }

    u8 ascii = codes[byte];

    keyboard_add_press(ascii);

    if (ascii != 0) {
        kb_buff[kb_buff_hd] = ascii;
        kb_buff_hd = next_hd;
        return;
    }
}

static void keyboard_callback() {
    pull_input();
}

u8 keyboard_test(u8 status) {
    u8 output_buffer = (status & ~(1 << 0));
    u8 input_buffer = (status & ~(1 << 1));
    u8 system_flag = (status & ~(1 << 2));
    u8 command_data = (status & ~(1 << 3));
    u8 time_out = (status & ~(1 << 6));
    u8 parity_error = (status & ~(1 << 7));


    if (output_buffer && input_buffer && system_flag && command_data && time_out && parity_error)
        return 0;
    else
        return -1;
}

char get_ascii_char() {
    while (1) {
        cli();

        if (kb_buff_hd != kb_buff_tl) {
            char c = kb_buff[kb_buff_tl];

            kb_buff_tl = (kb_buff_tl + 1) % 128;

            pull_input();

            sti();

            return c;
        }

        sti();
    }
}

void keyboard_setup() {
    outportb(0x64, 0xFF);

    u8 status = inportb(0x64);

    printf("[*] Got status (0x%x) after reset.\n", status);

    status = inportb(0x64);

    if (keyboard_test(status) == 0) {
        outportb(0x64, 0xAA); // test the controller

        u8 result = inportb(0x60);

        if (result == 0x55) {
            printf("[*] PS/2 controller test passed.\n");
        } else if (result == 0xFC) {
            printf("[*] PS/2 controller test failed.\n");
        } else {
            printf("[*] PS/2 controller test returned unknown error code: 0x%x\n", result);
            return;
        }

        outportb(0x64, 0x20);

        result = inportb(0x60);

        printf("[*] PS/2 config byte: 0x%x\n\n", result);

        register_interrupt_handler(IRQ1, keyboard_callback);
    } else
        panic("Failed to setup keyboard");
}