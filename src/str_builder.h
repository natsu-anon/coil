#ifndef STR_BUILDER_H
#define STR_BUILDER_H

char *join(char** str, const char c, const int len);

// NOTE: stop using this.  use file_sys file_txt -- no buffer memery
char *str_file(const char* abs_path, int* out_len);

char *sb_start();

void sb_build(char** buf, int* len, const char* str);

void shrink(char** buf, const int len);

#endif /* End STR_BUILDER_H */
