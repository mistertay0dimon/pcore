#ifndef PS2_H
#define PS2_H

#define BUFFER_SIZE 256

extern char buffer[BUFFER_SIZE];
char* read_line(void);
int read_int(void);

#endif
