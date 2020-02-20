#include <system/devices/display.h>
#include <system/chelpers.h>

#include <drivers/textmode.h>

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

char *convert(unsigned int num, int base) 
{ 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 
	
	ptr = &buffer[49]; 
	*ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0); 
	
	return(ptr); 
}

void printf(const char* format, ...) {
    if (!format)
        return;

    unsigned int a;
    char *traverse;

    va_list arg;

    va_start(arg, format);

    for (int i = 0; i < strlen(format); i++) {
        if (format[i] != '%') {
            putc(format[i], -1, -1);
            continue;
        } else {
            switch (format[i += 1]) {
                case 'c': {
                    int c = va_arg(arg, int);

                    putc(c, -1, -1);

                    continue;
                }

                case 'd': {
                    int d = va_arg(arg, int);

                    if (d < 0) {
                        d = -d;
                        puts("-");
                    }

                    puts(convert(d, 10));

                    continue;
                }

                case 'o': {
                    unsigned int o = va_arg(arg, unsigned int);

                    puts(convert(o, 8));

                    continue;
                }

                case 's': {
                    char *s = va_arg(arg, char*);

                    puts(s);

                    continue;
                }

                case 'x': {
                    unsigned int x = va_arg(arg, unsigned int);

                    puts(convert(x, 16));

                    continue;
                }

                default:
                    continue;
            }
        }
    }

    va_end(arg);
}

