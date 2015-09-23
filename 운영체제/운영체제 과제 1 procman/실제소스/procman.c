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

#include "procman.h"


char** str_split(char* a_str, const char a_delim);

void file_open(char **argv)
{
	char* file_name;
	file_name = strdup(argv[1]);
	
	argv_file = fopen(file_name,"r");

	if (argv_file == NULL)
	{
		fprintf(stderr,"config-file open error\n");
		return;
	}
	
	read_config_file();
}
/**
 * config파일을 읽고 파싱과 적절한 구조체 생성을 지시하는 함수입니다.
 */
void read_config_file()
{
	size_t size = 0; //getline함수를 쓰기위해서 존재하는 변수입니다. 이 안에 값이 쓰일 일은 없습니다.
	read_new_line_letter();	//이 함수로 config파일의 줄 수를 확인하고 저장합니다.
	input_string_array = calloc(line_many,sizeof(char *));
	pass_str_array = calloc(line_many,sizeof(passed_string *));

	int line_index;
	for (line_index = 0; !feof(argv_file); line_index++) //한줄씩 읽고 파싱합니다.
	{
		getline(&input_string_array[line_index],&size,argv_file);
		if (input_string_array[line_index][0] != '#' && strlen(input_string_array[line_index]) > 0)
		{
			//이 줄은 주석이 아닙니다. 또한 빈 줄도 아닙니다. 따라서 바로 파싱에 들어갑니다.
			
			//뭐하냐 몸통아 냉큼 파싱 함수 안 만들고! 뭐? 나보고 만들라고? 하여간 요즘 몸들은... 알겠다 strsep()가 한다!
		}
		else
		{
			printf("필요가 없는 줄입니다. --------- %s",input_string_array[line_index]);
		}
	}
	
	return;
}

/**
 * \n의 갯수 = 행의 갯수를 읽는 함수입니다. 이후에 이 행의 갯수 만큼 passed_string 구조체의 배열을 동적으로 할당할 것입니다.
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
