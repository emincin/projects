#include <termios.h>
#include <unistd.h>

#include <stdio.h>

void enable_raw_mode(void) {
    int ret = 0;
    struct termios io = {0};
    ret = tcgetattr(STDIN_FILENO, &io);
}

void disable_raw_mode(void) {
}

int main(int argc, char** argv) {
    enable_raw_mode();
    disable_raw_mode();
    return 0;
}
