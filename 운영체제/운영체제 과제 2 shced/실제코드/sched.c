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

#include "sched.h"

/**
 * \n의 갯수 = 행의 갯수를 읽는 함수입니다. 이후에 이 행의 갯수 만큼 parseed_string 구조체의 배열을 동적으로 할당할 것입니다.
 */ 
void read_new_line_letter()
{
	line_many = 0;
	char * message = NULL;
	size_t size = 0;
	
	while (!feof(argv_file))
	{
		getline(&message,&size,argv_file);
		line_many++;
	}
	
	free(message);
	
	//이제 파일의 지시자를 맨 앞으로 옮깁시다.
	fseek(argv_file,0,SEEK_SET);
}

/**
* data 파일을 읽고, 프로세스의 갯수, 파싱과정을 거치면서 분석에 필요한 데이터를 모읍니다.
*/
void read_data_file()
{
	size_t size = 0; //getline함수를 쓰기위해서 존재하는 변수입니다. 이 안에 값이 쓰일 일은 없습니다.
	read_new_line_letter();
	input_string_array = calloc(line_many,sizeof(char*));
	parsed_str_array = calloc(line_many,sizeof(parsed_string));
	proc_run_array = calloc(line_many,sizeof(process_running));

	int line_index; //각 줄을 탐색하는데 쓰일 인덱스입니다.
	for (line_index = 0; !feof(argv_file); line_index++)
	{
		getline(&input_string_array[line_index],&size,argv_file);
		printf("%s",input_string_array[line_index]);
	}
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

	read_new_line_letter(argv_file);
	
	read_data_file();
}




int main (int argc, char **argv)
{
	if (argc <= 1)
	{
		fprintf (stderr, "usage: %s input-file\n", argv[0]);
		return -1;
	}
	
	file_open(argv);

	/* ... */

	return 0;
}
