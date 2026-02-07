#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SN(s) (s), (sizeof(s) - 1)

typedef int8_t i8;
typedef uint8_t u8;

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

typedef intptr_t isize;
typedef uintptr_t usize;

typedef struct Size {
    int width;
    int height;
} Size;

typedef struct StringView {
    usize size;
    const char* data;
} StringView;

typedef struct InputEvent {
    int type;
} InputEvent;

StringView make_sv(const char* s, usize n) {
    return (StringView){.data = s, .size = n};
}

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
    isize ret = 0;
    ret = read(STDIN_FILENO, s, n);
    return ret;
}

isize write_sn(const char* s, usize n) {
    isize ret = 0;
    ret = write(STDOUT_FILENO, s, n);
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

bool input_available(void) {
    bool available = false;
    int bytes = 0;
    int ret = ioctl(STDIN_FILENO, FIONREAD, &bytes);
    available = bytes > 0;
    return available;
}

void enable_alternate_buffer(void) {
    write_sn(SN("\033[?1049h"));
}

void disable_alternate_buffer(void) {
    write_sn(SN("\033[?1049l"));
}

void enable_mouse_support(void) {
    write_sn(SN("\033[?1000;1003;1006h"));
}

void disable_mouse_support(void) {
    write_sn(SN("\033[?1000;1003;1006l"));
}

void enable_cursor(void) {
    write_sn(SN("\033[?25h"));
}

void disable_cursor(void) {
    write_sn(SN("\033[?25l"));
}

void set_cursor_pos(int x, int y) {
    char buf[32] = {0};
    int ret = snprintf(buf, sizeof(buf), "\033[%d;%dH", y + 1, x + 1);
    if (ret > 0) {
        write_sn(buf, ret);
    }
}

InputEvent parse_input_ansi(const char* s, usize n, usize* pos) {
    InputEvent event = {0};
    *pos = 0;
    usize index = 0;
    while (index < n) {
        char c = s[index];
        if (c == 27) {
        }
        index++;
    }
    return event;
}

i64 get_file_size(const char* path) {
    i64 file_size = -1;
    struct stat info = {0};
    int ret = stat(path, &info);
    if (ret == 0) {
        file_size = info.st_size;
    }
    return file_size;
}

void wait_ms(u32 ms) {
    usleep(ms * 1000);
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

void test_raw_mode_2(void) {
    enable_alternate_buffer();
    enable_raw_mode();
    enable_mouse_support();
    disable_cursor();
    while (true) {
        char buf[1024] = {0};
        int ret = (int)read_sn(buf, sizeof(buf));
        if (ret == 1 && buf[0] == 27) {
            break;
        }
        for (int i = 0; i < ret; i++) {
            char c = buf[i];
            if (is_printable_char(c)) {
                printf("%c", c);
            } else {
                printf("(%d)", c);
            }
        }
        printf("\n");
    }
    enable_cursor();
    disable_mouse_support();
    disable_raw_mode();
    disable_alternate_buffer();
}

void test_text_editor(void) {
    enable_alternate_buffer();
    enable_raw_mode();
    enable_mouse_support();
    disable_cursor();
    while (true) {
        if (input_available()) {
            char buf[1024] = {0};
            isize ret = read_sn(buf, sizeof(buf));
            if (ret < 0) {
                break;
            }
            usize pos = 0;
            while (true) {
                InputEvent event = parse_input_ansi(buf, ret, &pos);
                if (pos == 0) {
                    break;
                }
            }
        }
        wait_ms(100);
    }
    enable_cursor();
    disable_mouse_support();
    disable_raw_mode();
    disable_alternate_buffer();
}

int main(int argc, char** argv) {
    return 0;
}
