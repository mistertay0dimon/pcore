#include "echo_parser.h"

/**
 * @brief Checks if a character is a whitespace (space or tab)
 */
static int is_whitespace(char c) {
    return c == ' ' || c == '\t';
}

/**
 * @brief Prints the string as-is, including spaces
 */
static void print_token(const char* start, int len) {
    for (int i = 0; i < len; i++) {
        print_char(start[i]);
    }
}

/**
 * @brief Parses the line after "echo " and prints tokens.
 * Supports quoted strings.
 */
void echo_parse(const char* line) {
    const char* p = line;

    // Skip initial "echo" keyword
    while (*p && !is_whitespace(*p)) p++;
    while (*p && is_whitespace(*p)) p++;

    while (*p) {
        if (*p == '"') {
            // Quoted string
            p++; // skip opening quote
            const char* start = p;
            while (*p && *p != '"') p++;
            print_token(start, p - start);
            if (*p == '"') p++; // skip closing quote
        } else {
            // Unquoted token
            const char* start = p;
            while (*p && !is_whitespace(*p)) p++;
            print_token(start, p - start);
        }

        // Print space between tokens if more
        while (*p && is_whitespace(*p)) p++;
        if (*p) print_char(' ');
    }
}