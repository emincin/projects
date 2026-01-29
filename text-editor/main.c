#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SN(s) (s), (sizeof(s) - 1)

typedef intptr_t isize;
typedef uintptr_t usize;

typedef struct {
    int width;
    int height;
} Size;

bool is_control_char(char c) {
    if (0 <= c && c <= 31) {
        return true;
    }
    if (c == 127) {
        return true;
    }
    return false;
}

bool is_printable_char(char c) {
    if (32 <= c && c <= 126) {
        return true;
    }
    return false;
}

int get_width_of_char(char c) {
    if (is_printable_char(c)) {
        return 1;
    }
    return 0;
}

void enable_raw_mode(void) {
    int ret = 0;
    struct termios io = {0};
    ret = tcgetattr(STDIN_FILENO, &io);
    io.c_iflag &= ~(ICRNL);
    io.c_iflag &= ~(IXON);
    io.c_lflag &= ~(ECHO);
    io.c_lflag &= ~(ICANON);
    io.c_lflag &= ~(IEXTEN);
    io.c_lflag &= ~(ISIG);
    ret = tcsetattr(STDIN_FILENO, TCSANOW, &io);
}

void disable_raw_mode(void) {
    int ret = 0;
    struct termios io = {0};
    ret = tcgetattr(STDIN_FILENO, &io);
    io.c_iflag |= (ICRNL);
    io.c_iflag |= (IXON);
    io.c_lflag |= (ECHO);
    io.c_lflag |= (ICANON);
    io.c_lflag |= (IEXTEN);
    io.c_lflag |= (ISIG);
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

Size get_window_size(void) {
    Size size = {0};
    int ret = 0;
    struct winsize ws = {0};
    ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    size.width = ws.ws_col;
    size.height = ws.ws_row;
    return size;
}

void enable_alternate_buffer(void) {
    write_sn(SN("\033[?1049h"));
}

void disable_alternate_buffer(void) {
    write_sn(SN("\033[?1049l"));
}

void test_raw_mode(void) {
    enable_raw_mode();
    while (true) {
        int ret = 0;
        char buf[1024] = {0};
        ret = (int)read_sn(buf, sizeof(buf));
        for (int i = 0; i < ret; i++) {
            char c = buf[i];
            if (is_printable_char(c)) {
                printf("%c(%d)\n", c, c);
            } else {
                printf("%d\n", c);
            }
        }
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
