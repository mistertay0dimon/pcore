#include <pcorefn.h>
#include <ps2.h>

char buffer[BUFFER_SIZE];

static char scancode_map[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' '
};

static char scancode_map_shift[] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"', '~',
    0,'|','Z','X','C','V','B','N','M','<','>','?', 0,'*', 0,' '
};

static int shift_down = 0;

static char sc_to_ascii(uint8_t sc) {
    if (sc > 58) return 0;

    if (shift_down)
        return scancode_map_shift[sc];

    return scancode_map[sc];
}

char* read_line(void) {
    int pos = 0;

    while (1) {
        while (!(inb(0x64) & 1));

        uint8_t sc = inb(0x60);

        // Shift press
        if (sc == 0x2A || sc == 0x36) {
            shift_down = 1;
            continue;
        }

        // Shift release
        if (sc == (0x2A | 0x80) || sc == (0x36 | 0x80)) {
            shift_down = 0;
            continue;
        }

        if (sc == 0x1C) {
            buffer[pos] = 0;
            print("\n");
            return buffer;
        }

        if (sc == 0x0E) {
            if (pos > 0) {
                pos--;
                print("\b \b");
            }
            continue;
        }

        if (sc & 0x80)
            continue;

        char c = sc_to_ascii(sc);
        if (c) {
            if (pos < BUFFER_SIZE - 1) {
                buffer[pos++] = c;
                print_char(c);
            }
        }
    }
}

int read_int(void) {
    char* line = read_line();
    int result = 0;
    int i = 0;
    int sign = 1;

    while (line[i] == ' ') i++;

    if (line[i] == '-') {
        sign = -1;
        i++;
    } else if (line[i] == '+') {
        i++;
    }

    while (line[i] >= '0' && line[i] <= '9') {
        result = result * 10 + (line[i] - '0');
        i++;
    }

    return result * sign;
}
