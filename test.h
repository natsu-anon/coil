#ifndef COIL_TEST_H
#define COIL_TEST_H

#ifdef _WIN32
typedef struct _iobuf FILE;
#else
typedef struct _IO_FILE FILE;
#endif

typedef struct bruh bruh;

typedef struct fam /* this should be ignored */
{
} fam;

static int a = 22;

void foo(int a);

void *bar(int a);

bruh* guh(int a);

static inline fam lol(int a)     /* WHAT ABOUT THIS???? */              ;

static inline void aaaaaa(int lmao)
{
}

#endif /* End COIL_TEST_H */
