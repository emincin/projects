#include <termios.h>
#include <unistd.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef intptr_t isize;
typedef uintptr_t usize;

bool is_control_char(char c) {
    if (0 <= c && c <= 31) {
        return true;
    }
    if (c == 127) {
        return true;
    }
    return false;
}

void enable_raw_mode(void) {
    int ret = 0;
    struct termios io = {0};
    ret = tcgetattr(STDIN_FILENO, &io);
    ret = tcsetattr(STDIN_FILENO, TCSANOW, &io);
}

void disable_raw_mode(void) {
    int ret = 0;
    struct termios io = {0};
    ret = tcgetattr(STDIN_FILENO, &io);
    ret = tcsetattr(STDIN_FILENO, TCSANOW, &io);
}

isize read_sn(char* s, usize n) {
    ssize_t ret = read(STDIN_FILENO, s, n);
    return ret;
}

isize write_sn(const char* s, usize n) {
    ssize_t ret = write(STDOUT_FILENO, s, n);
    return ret;
}

void test_raw_mode(void) {
    enable_raw_mode();
    while (true) {
        int ret = 0;
        char buf[1024] = {0};
        ret = (int)read_sn(buf, sizeof(buf));
    }
    disable_raw_mode();
}

void test_text_editor(void) {
    enable_raw_mode();
    disable_raw_mode();
}

int main(int argc, char** argv) {
    return 0;
}
