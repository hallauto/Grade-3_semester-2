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
	correct_process_many = 0; //정확한 프로세스 갯수를 세는데 쓰이는 전역변수를 초기화 합니다.
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

int check_id(char* seperated_string)
{
	return 0;
}

int check_arrive_time(char* seperated_string)
{
	return 0;
}

int check_service_time(char* seperated_string)
{
	return 0;
}

int check_priority(char* seperated_string)
{
	return 0;
}

void parse_string(int line_index)
{
	char * copied_string = NULL; //strsep함수를 사용하기위해서는 내용이 변해도 문제없는 문자열이 필요합니다. 이를 위해 strdup함수를 이용해서 문자열을 복제할 것입니다.
	char * seperated_string = NULL; //strsep함수로 분리된 문자열은 여기에 저장됩니다. 이후에 값을 parse_str_array의 원소에 저장할 것입니다.
	char * delimiter = " "; //구분자 변수입니다.
	
	if (strlen(input_string_array[line_index]) == 0 || input_string_array[line_index][0] == '#')
	{
		return;
	}
	
	seperated_string = strsep(&copied_string, delimiter);
	if (check_id(seperated_string)) //먼저 id의 형식이 맞는지 확인합니다. 문제가 있으면 if는 참입니다.
	{
		fprintf(stderr,"invalid process id ‘%s’ in line %d, ignored",seperated_string, line_index);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return;
	}
	
	//문제가 없으면 id를 구조체에 저장합니다.
	strcpy(parsed_str_array[line_index].program_id,seperated_string);
	
	seperated_string = strsep(&copied_string,delimiter);
	if (check_arrive_time(seperated_string))
	{
		fprintf(stderr,"invalid arrive-time ‘%s’ in line %d, ignored",seperated_string, line_index);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return;
	}
	
	parsed_str_array[line_index].arrive_time = atoi(seperated_string);
	
	seperated_string = strsep(&copied_string,delimiter);
	if (check_service_time(seperated_string))
	{
		fprintf(stderr,"invalid service-time ‘%s’ in line %d, ignored",seperated_string, line_index);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return;
	}
	
	parsed_str_array[line_index].service_time = atoi(seperated_string);
	
	seperated_string = strsep(&copied_string,delimiter);
	if (check_priority(seperated_string))
	{
		fprintf(stderr,"invalid priority ‘%s’ in line %d, ignored",seperated_string, line_index);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return;
	}
	
	parsed_str_array[line_index].priority = atoi(seperated_string);
	
	
	
	correct_process_many++;
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
		fprintf (stderr, "input file must specified");
		return -1;
	}
	
	file_open(argv);

	/* ... */

	return 0;
}
