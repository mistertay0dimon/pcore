#ifndef BASIC_H
#define BASIC_H

#define VERSION "1.0.0"
#define MAX_VARS 32

struct Var {
    char name[16];
    int value;
};

extern struct Var vars[MAX_VARS];
extern int var_count;

void BasicStart();
#endif