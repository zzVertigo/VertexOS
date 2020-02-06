#include <system/display.h>
#include <system/cstring.h>

#include <display/textmode.h>

char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

#define MAX_PRECISION	(10)

static const double rounders[MAX_PRECISION + 1] =
{
	0.5,				// 0
	0.05,				// 1
	0.005,				// 2
	0.0005,				// 3
	0.00005,			// 4
	0.000005,			// 5
	0.0000005,			// 6
	0.00000005,			// 7
	0.000000005,		// 8
	0.0000000005,		// 9
	0.00000000005		// 10
};

void itoa(unsigned i, unsigned base, char* buf) {
    int pos = 0;
    int opos = 0;
    int top = 0;

    if (i == 0 || base > 16) {
        buf[0] = '0';
        buf[1] = '\0';

        return;
    }

    while (i != 0) {
        tbuf[pos] = bchars[i % base];
        pos++;

        i /= base;
    }

    top = pos--;

    for (opos = 0; opos < top; pos--, opos++) {
        buf[opos] = tbuf[pos];
    }

    buf[opos] = 0;
}

void itoa_s(int i, unsigned base, char* buf) {
    if (base > 16)
        return;

    if (i < 0) {
        *buf++ = '-';
        i *= -1;
    }

    itoa(i, base, buf);
}

int pow(int base, int exponent) {
    int result = 1;

    for (exponent; exponent > 0; exponent--) {
        result = result * base;
    }

    return result;
}

void reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
}

int intToStr(int x, char str[], int d) 
{ 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
  
    // If number of digits required is more, then 
    // add 0s at the beginning 
    while (i < d) 
        str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
}

void ftoa(double n, char *res, int afterpoint) {
    int ipart = (int)n;

    float fpart = n - (float)ipart;

    int i = intToStr(ipart, res, 0);

    n += rounders[afterpoint];

    res[i] = '.';

    fpart = fpart * pow(10, afterpoint);

    intToStr((int)fpart, res + i + 1, afterpoint);
}

void kprintf(const char* string, ...) {
    if (!string)
        return;

    va_list ap;

    va_start(ap, string);

    for (size_t i = 0; i < strlen(string); i++) {
        if (string[i] == '%') {
            switch (string[i + 1]) {
                case 's': {
                    string = va_arg(ap, char*);
                    textmode_puts(string);
                    i++;
                    continue;
                }

                case 'd': {
                    int s = va_arg(ap, int);
                    char str[32] = {0};
                    itoa_s(s, 10, str);
                    textmode_puts(str);
                    i++;
                    continue;
                }

                case 'x': {
                    int s = va_arg(ap, int);
                    char str[32] = {0};
                    itoa(s, 16, str);
                    textmode_puts(str);
                    i++;
                    continue;
                }

                case 'c': {
                    char s = (char)(va_arg(ap, int) & ~0xFFFFFF00);
                    textmode_putc(s);
                    i++;
                    continue;
                }

                case 'f': {
                    double s = va_arg(ap, double);
                    char str[32] = {0};
                    ftoa(s, str, 6);
                    textmode_puts(str);
                    i++;
                    continue;
                }

                default:
                    break;
            }
        } else {
            textmode_putc(string[i]);
        }
    }

    va_end(ap);
}

