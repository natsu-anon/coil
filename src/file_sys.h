#ifndef FILE_SYS_H
#define FILE_SYS_H

#ifdef _WIN32
typedef struct _iobuf FILE;
#else
typedef struct _IO_FILE FILE;
#endif

char *fs_abs_path(const char *rel_path);

int fs_split_path(const char *path, char **dirname, char **filename);

#endif /* End FILE_SYS_H */
