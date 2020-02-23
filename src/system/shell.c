#include <system/shell.h>
#include <system/devices/display.h>
#include <system/chelpers.h>

const char *username = "vertigo";
const char *hostname = "vertex";

void setup_shell() {
    printf("%s [Version %d.%d.%d]\n", NAME, MAJOR, MINOR, BUILD);
    printf("(c) %d %s. All rights reserved.\n\n", COPYRIGHT_YEAR, COPYRIGHT_OWNER);

    printf("%s@%s:~$ ", username, hostname);
}

void handle_command(u8* command_buffer) {
}