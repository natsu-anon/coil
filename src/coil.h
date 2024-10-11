#ifndef COIL_H
#define COIL_H

#ifdef _WIN32
typedef struct _iobuf FILE;
#else
typedef struct _IO_FILE FILE;
#endif

typedef unsigned uint32_t;

void coil_define_c(FILE *buf, const char *src, const int len);

void coil_define_cpp(FILE *buf, const char *src, const int len, const uint32_t r0, const uint32_t r1);

#endif /* End COIL_H */
