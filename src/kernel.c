#include "pcorefn.h"
#include "ps2.h"
#include "echo_parser.h"
#include "basic.h"

#define OS_NAME "PCore"
#define MAX_BASIC_LINE 256

void uptime_command(int seconds) {
    print("Uptime: ");
    intprint(seconds);
    print("\n");
}

void KMainFn() {
    ClearScreen();
    int seconds = 0;

    print(OS_NAME);
    print("\n");
    print("Type 'help' for help.\n");

    while (1) {
        sleep(1000);
        seconds++;

        print("pcore> ");
        char* cmd = read_line();

        if (strcmp(cmd, "clear") == 0) {
            ClearScreen();
        } else if (strcmp(cmd, "help") == 0) {
            print("Avabiable commands:\n");
            print("help - This command prints help. You already runned it.\n");
            print("clear - Clear screen.\n");
            print("exit - Turn off the system.\n");
            print("echo your string - Print text to screen.\n");
            print("reboot - Reboot the system.\n");
            print("uptime - Seconds from starting kernel.\n");
            print("info - System info.\n");
            print("calc - Calculator program.\n");
            print("basic - The BASIC interpreter.\n");
            print("panic - Kernel Panic test.\n");
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "reboot") == 0) {
            while (1) {
                outb(0x64, 0xFE);

                print("Failed to reboot. System halted.");
                while (1) {
                    asm volatile ("hlt");
                }
            }
        } else if (strcmp(cmd, "uptime") == 0) {
            uptime_command(seconds);
        } else if (strcmp(cmd, "info") == 0) {
            print("System Information\n");
            print("OS: ");

            print(OS_NAME);
            print("\n");

            uptime_command(seconds);
        } else if (strcmp(cmd, "calc") == 0) {
            int result = 0;
            print("Calculator\n");

            print("Enter first number: ");
            int first_num = read_int();
            print("Enter second number: ");
            int second_num = read_int();
            print("Operation (+, -, /, *): ");
            char* operation = read_line();
            if (strcmp(operation, "+") == 0) {
                result = first_num + second_num;
            } else if (strcmp(operation, "-") == 0) {
                result = first_num - second_num;
            } else if (strcmp(operation, "/") == 0) {
                if (second_num == 0) {
                    print("Division by zero is not allowed.\n");
                } else {
                    result = first_num / second_num;
                }
            } else if (strcmp(operation, "*") == 0) {
                result = first_num * second_num;
            } else {
                print("Unknown operation.\n");
            }

            print("Result: ");
            intprint(result);
            print("\n");
        } else if (strcmp(cmd, "basic") == 0) {
            BasicStart();
        } else if (strcmp(cmd, "panic") == 0) {
            print("In EAX register the meaning will change. This is for registers show test.\n");
            asm volatile("movl $0x1, %eax");
            panic("Executed command 'panic' for kernel panic test.");
        } else if (cmd[0] == 0) {
            continue; // Doing nothing (as in most systems)
        } else {
            // Check for "echo" command
            const char* echo_prefix = "echo";
            int i = 0;
            while (cmd[i] && echo_prefix[i] && cmd[i] == echo_prefix[i]) i++;
            if (i == 4 && (cmd[i] == ' ' || cmd[i] == '\0')) {
                echo_parse(cmd);
                print("\n");
            } else {
                print("Unknown command: ");
                print(cmd);
                print("\n");
            }
        }
    }

    shutdown();
}