#include "str_builder.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> /* for file descriptor flags */
#include <stdlib.h>
#include <string.h>
#include "error_macros.h"
#include "file_sys.h"

// TODO use assertf more

#define SB_SIZE0 1024

int double_buffer(char** buf, const int size)
{
	char* temp = realloc(*buf, 2 * size);
	assertf(temp, "realloc() error in str_builder.c:_double_buffer()");
	*buf = temp;
	return 2 * size;
}


char *join(char** str, const char c, const int strc)
{
	char* res;
	if (strc == 0)
	{
		res = NULL;
		return res;
	}
	int n = strlen(str[0]);
	int len = 0;
	int res_size = SB_SIZE0 > n ? SB_SIZE0 : n;
	res = malloc(res_size * sizeof *res);
	memmove(res, str[0], n);
	len += n;
	for (int i = 1; i < strc; ++i)
	{
		n = strlen(str[i]);
		if (len + n + 1 > res_size)
		{
			res_size = double_buffer(&res, res_size);
		}
		res[len] = c;
		memmove(res + len + 1, str[i], n);
		len += n + 1;
		printf("%s\n", str[i]);

	}
	shrink(&res, len);
	return res;
}

/*
 * NOTE: justuse read() or fread()
 */
char* str_file(const char* rel_path, int* out_len)
{
	int fd;
	{
		char* abs_path = fs_abs_path(rel_path);
		fd = open(abs_path, O_RDONLY | O_EXCL);
		assertf(fd != -1, "Failed to acquire file descriptor for :%s.  Does it exist?", abs_path);
		free(abs_path);
	}
	int len = 0;
	int str_size = SB_SIZE0;
	char* res = malloc(str_size * sizeof *res);
	{
		ssize_t size_read;
		while ((size_read = read(fd, res + len, str_size - len * sizeof *res)))
		{
			assertf(size_read != -1, "read() error in str_builder.c:str_file()");
			len += size_read;
			if (len == str_size)
			{
				str_size = double_buffer(&res, str_size);
			}
		}
		len += size_read;
		*out_len = len;
		shrink(&res, len);
	}
	// TODO: handle this better
	assertf(close(fd) == 0, "Failed to close file handler for %s", rel_path);
	return res;
}

char* sb_start()
{
	return malloc(SB_SIZE0 * sizeof(char));
}

// NOTE: DOES NOT COPY '\0' -- shrink() BEFORE USING
void sb_build(char** buf, int* len, const char* str)
{
	int n = strlen(str);
	int l = *len;
	int k = (l + n / SB_SIZE0) - (l / SB_SIZE0); // INT DIV NOT COMMUTATIVE -- CANT GROUP
	if (k > 0)
	{
		char* temp = realloc(*buf, SB_SIZE0 * (k + (l / SB_SIZE0)));
		assertf(temp != NULL, "realloc() failed in sb_build()");
		*buf = temp;
	}
	memmove(*buf + l, str, n);
	*len = l + n;
}

void shrink(char** buf, const int len)
{
	char* temp = realloc(*buf, (len + 1) * sizeof *temp);
	assertf(temp != NULL, "realloc() failed in shrink()");
	*buf = temp;
	(*buf)[len] = '\0';
}
