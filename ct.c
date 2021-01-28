#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utime.h>

#define PE_SIGNATURE "00004550"

// reads four bytes in hex in little endian format.
int read_four_bytes_hex(char *str, FILE *src);

int main(int argc, char *argv[])
{
	FILE *fp;
	int c, i, idx;
	char temp[9];
	unsigned long n;
	
	time_t tm;
	struct utimbuf new_times;
	char* c_time_string;

	if(argc != 2)
	{
		fprintf(stderr, "usage: provide a command in the format ct <path to the windows executable file>. Eg: ct d:\\dev\\binaries\\mydll.dll");
		return 1;
	}

	fp = fopen(argv[1], "r");
	
	if(fp == NULL)
	{
		fprintf(stderr, "error: failed to open the file.");
		return 1;
	}
	
	for(i=1; i<=60; i++)
	{
		c = getc(fp);
		if(c == EOF)
		{
			fprintf(stderr, "error: the file is not a windows object file.");
			return -1;
		}
	}

	temp[8] = '\0';
	
	read_four_bytes_hex(temp, fp);
	i+= 4;

	n = strtoul(temp, NULL, 16);

	for(;i<n;i++)
	{
		c = getc(fp);
		if(c == EOF)
		{
			fprintf(stderr, "error: the file is not a windows object file.");
			return -1;
		}
	}

	if(read_four_bytes_hex(temp, fp) != 0)
	{
		fprintf(stderr, "error: the file is not a windows object file.");
		return -1;
	}
	i+=4;

	if(strcmp(temp, PE_SIGNATURE) != 0)
	{
		fprintf(stderr, "File is not a PE or its header is corrupted. Either way, bubye!");
		return -1;
	}

	i=0;

	for(;i<4;i++)
	{
		c = getc(fp);
		if(c == EOF)
		{
			fprintf(stderr, "error: the file is not a windows object file.");
			return -1;
		}
	}

	if(read_four_bytes_hex(temp, fp) != 0)
	{
		fprintf(stderr, "error: the file is not a windows object file.");
		return -1;
	}

	n = strtoul(temp, NULL, 16);

	tm = n;

	c_time_string = ctime(&tm);

	printf("Compile Date Time Stamp: %s", c_time_string);
	new_times.modtime = n;
	utime(argv[1], &new_times);

	fclose(fp);
	return 0;
}

// reads four bytes in hex in little endian format.
int read_four_bytes_hex(char *str, FILE *src)
{
	char byte_str[2];
	int i, c;
	str += 6;
	for(i=0;i<4;i++)
	{
		c = getc(src);

		if(c == EOF)
		{
			return -1;
		}

		sprintf(byte_str, "%002x ", c);
		strncpy(str, byte_str, 2);
		str -= 2;
	}
	return 0;
}
