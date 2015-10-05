/*
 * OS Assignment #2
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>

void read_data_file()
{
	
}

 /**
 * 주어진 argv를 이용해서 config파일을 오픈하는 함수입니다.
 */
void file_open(char **argv)
{
	char* file_name;
	file_name = strdup(argv[1]);
	
	argv_file = fopen(file_name,"r");

	if (argv_file == NULL)
	{
		fprintf(stderr,"failed to load data file ‘%s’\n",file_name);
		return;
	}
	
	read_data_file();
}




int main (int argc, char **argv)
{
	if (argc <= 1)
	{
		fprintf (stderr, "usage: %s input-file\n", argv[0]);
		return -1;
	}

	/* ... */

	return 0;
}
