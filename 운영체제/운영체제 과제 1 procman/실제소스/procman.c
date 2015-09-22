/*
 * OS Assignment #1
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>

void file_open(char **argv)
{
	char* file_name;
	file_name = strdup(argv[1]);
}


int main (int argc, char **argv)
{
	if (argc <= 1)
	{
		fprintf (stderr, "usage: %s config-file\n", argv[0]);
		return -1;
	}
	else
	{
		file_open(argv);
	}
	return 0;
}
