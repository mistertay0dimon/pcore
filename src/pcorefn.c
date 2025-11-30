#include "pcorefn.h"

static uint16_t* vram = (uint16_t*)VRAM_ADDR;
static int cursor_x = 0;
static int cursor_y = 0;

/**
 * @brief memcpy function realization.
 */

void memcpy(void *dest, const void *src, uint32_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    for (uint32_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

/**
 * @brief memset function realization.
 */

void memset(void *ptr, int val, uint32_t n) {
    uint8_t *p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < n; i++) {
        p[i] = (uint8_t)val;
    }
}

/**
 * @brief Function for setting cursor.
 * @param x X coordinate.
 * @param y Y coordinate.
 */

void SetCursor(int x, int y) {
    cursor_x = x;
    cursor_y = y;
    uint16_t pos = y * COLS + x;

    outb(VGA_COMMAND_PORT, 0x0F);
    outb(VGA_DATA_PORT, (uint8_t)(pos & 0xFF));

    outb(VGA_COMMAND_PORT, 0x0E);
    outb(VGA_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));
}

/**
 * @brief Function for printing character on the screen.
 * @param c Character.
 */

void print_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            vram[cursor_y * COLS + cursor_x] = (DEFAULT_ATTR << 8) | ' ';
        }
    } else {
        vram[cursor_y * COLS + cursor_x] = (DEFAULT_ATTR << 8) | c;
        cursor_x++;
        if (cursor_x >= COLS) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= ROWS) {
        for (int y = 1; y < ROWS; y++)
            for (int x = 0; x < COLS; x++)
                vram[(y-1)*COLS + x] = vram[y*COLS + x];
        for (int x = 0; x < COLS; x++)
            vram[(ROWS-1)*COLS + x] = (DEFAULT_ATTR << 8) | ' ';
        cursor_y = ROWS - 1;
    }

    SetCursor(cursor_x, cursor_y);
}

/**
 * @brief Function for printing string. It uses print_char function in cycle for print string, because strings - it's a characters massives.
 * @param str String to print
 */

void print(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

/**
 * @brief Function for clearing screen.
 */

void ClearScreen() {
    uint16_t blank = (DEFAULT_ATTR << 8) | ' ';
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            vram[y*COLS + x] = blank;
    cursor_x = 0;
    cursor_y = 0;
    SetCursor(cursor_x, cursor_y);
}

/**
 * @brief strcmp function realization. (function for compare strings)
 * @param a First string.
 * @param b Second string.
 */

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

/**
 * @brief Prints a integer (number).
 * @param n Number to print.
 */

void intprint(unsigned long n) {
    char buf[32];
    int i = 0;

    if (n == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        print(buf);
        return;
    }

    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = tmp;
    }

    buf[i] = '\0';
    print(buf);
}

/**
 * @brief Sleeping function by millseconds.
 * @param ms Millseconds.
 */

void sleep(unsigned long ms) {
    for (unsigned long i = 0; i < ms * 1000; i++) {
        asm volatile("nop");
    }
}

/**
 * @brief Function for the HEX number print.
 * @param val Value to print.
 */

void hexprint(uint32_t val) {
    print("0x");

    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (val >> (i * 4)) & 0xF;
        char c;

        if (nibble < 10)
            c = '0' + nibble;
        else
            c = 'A' + (nibble - 10);

        char s[2] = {c, 0};
        print(s);
    }
}

/**
 * @brief Kernel panic function.
 * @param reason Kernel panic reason.
 */

void panic(const char *reason) {
    regs_t r;
    get_regs(&r);
    r.eip = get_eip();

    print("Kernel panic.\n");
    print("An error has occurred during PCore work.\n");
    print("Crash information: \n\n");

    print("Reason: ");
    print(reason);
    print("\n");

    print("\n");

    print("Registers:\n");
    print("EAX: "); hexprint(r.eax); print("\n");
    print("EBX: "); hexprint(r.ebx); print("\n");
    print("ECX: "); hexprint(r.ecx); print("\n");
    print("EDX: "); hexprint(r.edx); print("\n");
    print("ESI: "); hexprint(r.esi); print("\n");
    print("EDI: "); hexprint(r.edi); print("\n");
    print("EBP: "); hexprint(r.ebp); print("\n");
    print("ESP: "); hexprint(r.esp); print("\n");
    print("EIP: "); hexprint(r.eip); print("\n");
    print("EFLAGS: "); hexprint(r.eflags); print("\n");

    print("\nSystem halted. Try restart your system.\n");

    asm volatile("cli; hlt");
}