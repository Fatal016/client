#ifndef H_BUFFER
#define H_BUFFER

#define MAX_BUF 2048

#include <stddef.h>

extern char buf[MAX_BUF];
extern size_t buf_pos;
extern size_t buf_len;

#endif
