#include "basic.h"
#include "pcorefn.h"
#include "ps2.h"

struct Var vars[MAX_VARS];
int var_count = 0;

static int starts_with(const char* s, const char* kw) {
    int i = 0;
    while (kw[i]) {
        if (s[i] != kw[i]) return 0;
        i++;
    }
    return 1;
}

static const char* skip(const char* s) {
    while (*s == ' ' || *s == '\t') s++;
    return s;
}

static int is_num(char c) {
    return c >= '0' && c <= '9';
}

static int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}

static int* get_var(const char* name) {
    for (int i = 0; i < var_count; i++) {
        int ok = 1;
        for (int j = 0; vars[i].name[j] || name[j]; j++) {
            if (vars[i].name[j] != name[j]) { ok = 0; break; }
        }
        if (ok) return &vars[i].value;
    }
    return 0;
}

static int* ensure_var(const char* name) {
    int* v = get_var(name);
    if (v) return v;
    if (var_count >= MAX_VARS) return 0;

    int idx = var_count++;
    int i = 0;
    while (name[i] && i < 15) {
        vars[idx].name[i] = name[i];
        i++;
    }
    vars[idx].name[i] = 0;
    vars[idx].value = 0;
    return &vars[idx].value;
}

static const char* parse_number(const char* s, int* out) {
    int v = 0;
    while (is_num(*s)) {
        v = v * 10 + (*s - '0');
        s++;
    }
    *out = v;
    return s;
}

static const char* parse_expr(const char* s, int* out);
static const char* parse_term(const char* s, int* out);
static const char* parse_factor(const char* s, int* out);

static const char* parse_var(const char* s, int* out) {
    char name[16];
    int i = 0;

    while (is_alpha(*s) && i < 15) {
        name[i++] = *s;
        s++;
    }
    name[i] = 0;

    int* v = get_var(name);
    *out = v ? *v : 0;
    return s;
}

static const char* parse_factor(const char* s, int* out) {
    s = skip(s);

    if (*s == '(') {
        s++;
        s = parse_expr(s, out);
        if (*s == ')') s++;
        return s;
    }

    if (is_num(*s))
        return parse_number(s, out);

    if (is_alpha(*s))
        return parse_var(s, out);

    *out = 0;
    return s;
}

static const char* parse_term(const char* s, int* out) {
    int v;
    s = parse_factor(s, &v);

    while (1) {
        s = skip(s);
        if (*s == '*') {
            int r; 
            s = parse_factor(s + 1, &r);
            v *= r;
        } else if (*s == '/') {
            int r; 
            s = parse_factor(s + 1, &r);
            if (r != 0) v /= r;
        } else break;
    }

    *out = v;
    return s;
}

static const char* parse_expr(const char* s, int* out) {
    int v;
    s = parse_term(s, &v);

    while (1) {
        s = skip(s);
        if (*s == '+') {
            int r; 
            s = parse_term(s + 1, &r);
            v += r;
        } else if (*s == '-') {
            int r; 
            s = parse_term(s + 1, &r);
            v -= r;
        } else break;
    }

    *out = v;
    return s;
}

static void exec_print(const char* line) {
    line += 5;
    line = skip(line);

    if (*line == '"') {
        line++;
        while (*line && *line != '"') {
            char c[2] = { *line, 0 };
            print(c);
            line++;
        }
        print("\n");
        return;
    }

    int v;
    parse_expr(line, &v);
    intprint(v);
    print("\n");
}

static void exec_let(const char* line) {
    line += 3;
    line = skip(line);

    char name[16];
    int i = 0;

    while (is_alpha(*line) && i < 15) {
        name[i++] = *line;
        line++;
    }
    name[i] = 0;

    line = skip(line);
    if (*line != '=') {
        print("Syntax error\n");
        return;
    }

    line++;
    int v;
    parse_expr(line, &v);

    int* dest = ensure_var(name);
    if (dest) *dest = v;
}

void BasicStart() {
    print("The BASIC interpreter ");
    print(VERSION);
    print("\n");

    while (1) {
        print("> ");
        char* line = read_line();
        if (line[0] == 0) continue;

        if (starts_with(line, "end")) return;

        if (starts_with(line, "print")) {
            exec_print(line);
            continue;
        }

        if (starts_with(line, "let")) {
            exec_let(line);
            continue;
        }

        print("Syntax error\n");
    }
}