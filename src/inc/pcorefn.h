#ifndef PCOREFN_H
#define PCOREFN_H

#define VRAM_ADDR 0xB8000
#define COLS 80
#define ROWS 25
#define COLOR_BLACK 0x0
#define COLOR_WHITE 0xF
#define DEFAULT_ATTR ((COLOR_BLACK << 4) | COLOR_WHITE)

#define VGA_COMMAND_PORT 0x3D4
#define VGA_DATA_PORT    0x3D5

#define TEXT_EDITOR_SAVE_ADDRESS 0x50000

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// Register struct
typedef struct regs_t {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp;
    uint32_t esp;   // Current stack
    uint32_t eip;
    uint32_t eflags;
} regs_t;

void SetCursor(int x, int y);
void print_char(char c);
void print(const char* str);
void ClearScreen();
int strcmp(const char* a, const char* b);
void memcpy(void *dest, const void *src, uint32_t n);
void memset(void *ptr, int val, uint32_t n);
void intprint(unsigned long n);
void sleep(unsigned long ms);
void panic(const char *reason);
void setcolor(uint8_t color);
void hexprint(uint32_t val);

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline void get_regs(regs_t *r) {
    asm volatile(
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        "mov %%esi, %4\n"
        "mov %%edi, %5\n"
        "mov %%ebp, %6\n"
        "mov %%esp, %7\n"
        "pushf\n"
        "pop %8\n"
        : "=m"(r->eax), "=m"(r->ebx), "=m"(r->ecx), "=m"(r->edx),
          "=m"(r->esi), "=m"(r->edi), "=m"(r->ebp),
          "=m"(r->esp), "=m"(r->eflags)
    );
}

static inline uint32_t get_eip() {
    uint32_t ip;
    asm volatile(
        "call 1f\n"
        "1: pop %0\n"
        : "=r"(ip)
    );
    return ip;
}

static inline void shutdown() {
    // --- ACPI try ---
    uint16_t value = 0x2000;
    uint16_t port  = 0x604;
    __asm__ volatile("outw %0, %1" :: "a"(value), "Nd"(port));

    // If ACPI not works, then:
    // --- APM try---
 
    __asm__ volatile(
        "movw $0x5307, %%ax\n\t"   // APM BIOS: Power Off Function
        "movw $0x0001, %%bx\n\t"   // APM connect
        "int $0x15\n\t"
        :
        :
        : "ax", "bx"
    );

    // If ACPI and APM not works, just halting in infinity cycle:
    while (1) {
        __asm__ volatile("hlt");
    }
}

#endif
