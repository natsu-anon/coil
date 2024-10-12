#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "file_sys.h"
#include "error_macros.h"
#include "str_builder.h"
#include "coil.h"


typedef enum CoilMode
{
	NONE = 0,
	ARGIN = 1 << 0,
	FILEIN = 1 << 1,
	STDOUT = 1 << 2,
	FILEOUT = 1 << 3,
	INCLUDE_HEADER = 1 << 4,
	CPP = 1 << 5
} CoilMode;


void include_header(FILE* buf, char* in_file, unsigned char c_mode)
{
	char* dir;
	char* file;
	if (fs_split_path(in_file, &dir, &file))
	{
		free(dir);
	}
	if (c_mode)
	{
		fprintf(buf, "#include \"%s\"\n", file);
		fprintf(buf, "/* #include <stdlib.h> */\n\n\n");
	}
	else {
		fprintf(buf, "#include \"%s\"\n\n\n", file);
	}
	/* fprintf(buf, "#include \"%s\"\n\n", file); */
	free(file);
}


int main(int argc, char** argv)
{
	CoilMode mode = ARGIN | STDOUT;
	char c;
	char* in_file;
	char* out_file;
	while((c = getopt(argc, argv, "hipo:c:")) != -1)
	{
		switch(c)
		{
		case 'h':
			printf("coil is a tool that defines C function declarations using tree-sitter.");
			return 0;
		case 'i':
			mode |= INCLUDE_HEADER;
			break;
		case 'p':
			mode |= CPP;
			break;
		case 'o':
			mode |= FILEIN;
			mode &= ~ARGIN;
			in_file = optarg;
			break;
		case 'c':
			mode |= FILEOUT;
			mode &= ~STDOUT;
			out_file = optarg;
			break;
		case ':':
			log_error("Unrecognized option: %s", argv[optind - 1]);
			return 1;
		case '?':
			log_error("Unrecognized option: %s", argv[optind - 1]);
			return 1;
		}
	}
	int len;
	char* src;
	if (mode & CPP)
	{
		if (~mode & FILEIN)
		{
			log_error("pass in C++ header file via -o!");
			return 1;
		}
		FILE* buf;
		if (mode & FILEOUT)
		{
			buf = fopen(out_file, "wa");
			assertf(buf, "Failed to acquire buffer for %s -- does it exist?", out_file);
		}
		else
		{
			buf = stdout;
		}
		char* abs_path = fs_abs_path(in_file);
		// src = str_file(abs_path, &len);
		src = file_text(abs_path, &len);
		free(abs_path);
		int r0 = 0;
		if (argc > optind)
		{
			r0 = atoi(argv[optind]);
			r0 = 0 > r0 ? 0 : r0;
		}
		int r1 = strlen(src);
		if (argc > optind + 1)
		{
			r1 = atoi(argv[optind + 1]);
			r1 = r1 < strlen(src) ? r1 : strlen(src) - 1;
		}
		if (mode & INCLUDE_HEADER && mode & FILEIN)
		{
			include_header(buf, in_file, 0);
		}
		coil_define_cpp(buf, src, len, r0, r1);
		free(src);
		fflush(buf);
		if (mode & FILEOUT)
		{
			fclose(buf);
		}
		return 0;
	}
	if (mode & FILEIN)
	{
		char* abs_path = fs_abs_path(in_file);
		src = file_text(abs_path, &len);
		free(abs_path);
	}
	else
	{
		src = join(&argv[optind], ' ', argc - optind);
		len = strlen(src);
	}
	FILE* buf;
	if (mode & FILEOUT)
	{
		buf = fopen(out_file, "wa");
		assertf(buf, "Failed to acquire buffer for %s -- does it exist?", out_file);
	}
	else
	{
		buf = stdout;
	}
	if (mode & INCLUDE_HEADER && mode & FILEIN)
	{
		include_header(buf, in_file, 1);
	}
	coil_define_c(buf,  src, len);
	free(src);
	fflush(buf);
	if (mode & FILEOUT)
	{
		fclose(buf);
	}
	return 0;
}
