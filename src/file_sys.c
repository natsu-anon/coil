#include "file_sys.h"
#include <stdlib.h>
#include <stdio.h>
/* #include <libgen.h> // this is NOT working well */
#include <string.h>
#include <limits.h> /* PATH MAX */
#include "error_macros.h"

#ifdef _WIN32
#define DIR_SEPERATOR '\\'
#define ABS_PATH(BUF, PATH, MAX_LEN) _fullpath(BUF, PATH, MAX_LEN)
#else /* NOTE: test this */
#define ABS_PATH(BUF, PATH, MAX_LEN) realpath(BUF, PATH)
#define DIR_SEPERATOR '/'
#endif


char *fs_abs_path(const char* rel_path)
{
	char *res = malloc(PATH_MAX * sizeof *res);
	if (ABS_PATH(res, rel_path, PATH_MAX) == NULL)
	{
		fprintf(stderr, "Failed to find absolute path for: %s", rel_path);
		exit(EXIT_FAILURE);

	}
	return res;
}


int fs_split_path(const char *path, char **dir, char **file)
{
	int len = strlen(path);
	for (int i = len; i >= 0; i--)
	{
		if (path[i] == DIR_SEPERATOR)
		{
			char *temp = malloc(i);
			memcpy(temp, path, i);
			temp[i] = '\0';
			*dir = temp;
			temp = malloc(len - i);
			memcpy(temp, path + i + 1, len - i);
			*file = temp;
			return 1;
		}
	}
	char *temp = malloc(len + 1);
	memcpy(temp, path, len);
	temp[len] = '\0';
	*dir = NULL;
	*file = temp;
	return 0;
}

// NOTE: use this instead of str_file
char* file_text(const char* abs_path, int* out_len)
{
	FILE* fp = fopen(abs_path, "r");
	assertf(fp, "Failed to open file stream for %s.  Does it exist?", abs_path);
	assertf(fseek(fp, 0, SEEK_END) > 0, "Failed to seek end position for file %s", abs_path);
	const int fsize = ftell(fp);
	rewind(fp);
	char* res = malloc(fsize * sizeof(char));
	fread(res, sizeof *res, fsize - 1, fp);
	res[fsize - 1] = '\n';
	fflush(fp);
	fclose(fp);
	return res;
}
