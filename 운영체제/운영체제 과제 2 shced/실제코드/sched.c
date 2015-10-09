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
 * 특정 문자열에서 특정 문자의 갯수를 세는 코드입니다.
 * char* string: 특정 문자를 포함한 문자열
 * char letter: 찾는 문자
 */
 int letter_cnt(char * string, char letter)
 {
	 int result = 0;
	 
	 while (*string) //string안에 값이 NULL이면 문자열이 끝난것이므로 루프가 끝납니다.
	 {
		 if (letter == * string)
		 {
			 result++;
		 }
		 string++;
	 }
	 
	 return result;
 }

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
	last_process_index = -1; //last_process_index는 시작할 때 -1을 삽입합니다. last_process_index가 -1 이라는 것은 아직 정상적인 프로세스 정보를 파싱한 적이 없다는 뜻입니다.
	size_t size = 0; //getline함수를 쓰기위해서 존재하는 변수입니다. 이 안에 값이 쓰일 일은 없습니다.
	read_new_line_letter();
	
	input_string_array = calloc(line_many,sizeof(char*));
	parsed_str_array = calloc(line_many,sizeof(parsed_string));
	proc_run_array = calloc(line_many,sizeof(process_running));

	int line_index; //각 줄을 탐색하는데 쓰일 인덱스입니다.
	for (line_index = 0; !feof(argv_file); line_index++)
	{
		getline(&input_string_array[line_index],&size,argv_file);
		if (input_string_array[line_index][0] != '#' && input_string_array[line_index][0] != '\n' && strlen(input_string_array[line_index]) > 0)
		{
			parse_string(line_index);
			
		}
	}
}

 /**
  * 문자열의 앞뒤 공백을 제거하는 함수입니다.
  * 어디까지나 앞과 뒤만 제거하므로, 사이에 있는 공백은 제거하지 않습니다.
  * char* string: 공백을 제거할 문자열입니다.
  */
 void remove_string_space(char* string)
 {
	 int string_length = strlen(string);
	 int string_index = 0;
	 int last_letter_index = 0; //탐색 중 마지막으로 문자가 있던 인덱스입니다.
	 int next_letter_index = 0; //last_letter_index가 변경된 이후, 처음으로 문자가 있던 인덱스입니다. 만일 이 인덱스가 strlen과 같다면, 이는 문자가 더 없다는 뜻입니다.
	 
	 for (string_index = 0; string_index < string_length; string_index++)
	 {
		 if (!isspace((int)string[string_index])) //공백이 아니면 last_letter_index값을 변경합니다.
		 {
			 last_letter_index = string_index;
		 }
		 else if (last_letter_index == 0) //공백이지만 아직까지 문자가 발견된 적은 없습니다. 즉, 문자열 앞부분에 공백이 있습니다. 이를 제거합니다.
		 {
			 int remove_index; //제거할 때 쓰일 인덱스입니다.
			 for (remove_index = string_index + 1; remove_index <= string_length; remove_index++) //현재 위치부터 이 뒤에 있는 값을 앞으로 넣어야합니다. 이를 위한 함수입니다.
			 {
				 string[remove_index - 1] = string[remove_index];
			 }
			 string_length--;
			 string_index--; //문자열을 한칸 앞으로 당겼으므로, 한번더 같은 곳을 검사해야합니다. 이를 위해 string_index를 1 감소시킵니다.
		 }
		 else //이번에는 공백이 문자열 사이에 있는지 끝에 있는지를 검사해야합니다.
		 {
			 if (next_letter_index < string_index) //next_letter_index가 string_index보다 작다면 이는 공백 다음에 문자가 있는지 탐색을 하지않았다는 뜻입니다. 탐색을 시작합니다.
			 {
				 for (next_letter_index = string_index; next_letter_index < string_length; next_letter_index++)
				 {
					 if (!isspace((int)string[next_letter_index])) //문자가 발견되었습니다. 바로 
						break;
				 }
				 
				 if (isspace((int)string[string_length-1]))
					next_letter_index++;
			 }
			 
			 if (next_letter_index >= string_length) //next_letter_index가 string_length보다 크거나 같다면 공백이 문자열 뒤에 존재한다는 뜻입니다. 바로 제거합니다.
			 {
				 string[last_letter_index+1] = string[string_length];
				 string_length = last_letter_index - 1;
			 }
		 }
	 }
 }

/**
 * id가 형식에 맞는지 체크합니다.
 * return: int 오류 코드
 * 0: 문제 없음
 * 1: 오류 발견 에러를 출력해야합니다.
 * 2: 중복된 아이디입니다.
 */
int check_id(char* seperated_string)
{
	int length = strlen(seperated_string); //검사할 문자열의 길이입니다.
	int check_index; //중복을 검색할 때 쓰는 인덱스입니다.
	if (length != 2)
	{
		return 1;
	}
	//일단 두 글자가 각각 영문자(소문자 포함)와 숫자인지 검사합니다.
	if (isalnum(seperated_string[0]) && isalnum(seperated_string[1]))
	{
		//이제 두 문자중에 소문자가 있는지 검사합니다. 이렇게 해서 id 형식 검사가 끝납니다.
		if (97 <= seperated_string[0] && seperated_string[0] <= 122)
		{
			return 1;
		}
		if (97 <= seperated_string[1] && seperated_string[1] <= 122)
		{
			return 1;
		}
	
		//형식이 맞다는 것을 확인했으니 이제 중복인지 아닌지 검사합니다.
		for (check_index = 0; check_index < line_many; check_index++)
		{
			if (strcmp(seperated_string, parsed_str_array[check_index].program_id) == 0)
			{
				return 2;
			}
		}
		
		return 0;
	}
	
	//길이가 2가 아니라면 이는 잘못된 형식의 id입니다.
	return 1;
}

/**
 * arrive_time이 형식에 맞는지 체크합니다. 이 때, last_process_index가 -1이 아니면 이 값을 인덱스로 parsed_str_array에 접근해서 arrive_time을 비교합니다.
 * return: int 오류 코드
 * 0: 문제 없음
 * 1: 오류 발견 에러를 출력해야합니다.
 */
int check_arrive_time(char* seperated_string)
{
	int arrive_time = atoi(seperated_string);
	if (0 <= arrive_time && arrive_time <= 30)
	{
		if (last_process_index >= 0)
		{
			if (arrive_time < parsed_str_array[last_process_index].arrive_time)
			{
				return 1;
			}
		}
		return 0; //문제가 없습니다. 다음으로 넘어갑니다.
	}	
	return 1;
}


/**
 * service time이 형식에 맞는지 체크합니다.
 * return: int 오류 코드
 * 0: 문제 없음
 * 1: 오류 발견 에러를 출력해야합니다.
 */
int check_service_time(char* seperated_string)
{
	int service_time = atoi(seperated_string);
	if (1 <= service_time && service_time <= 30)
	{
		return 0;
	}
	return 1;
}


/**
 * priority가 형식에 맞는지 체크합니다.
 * return: int 오류 코드
 * 0: 문제 없음
 * 1: 오류 발견 에러를 출력해야합니다.
 */
int check_priority(char* seperated_string)
{
	int priority = atoi(seperated_string);
	if (1 <= priority && priority <= 10)
	{
		return 0;
	}
	return 1;
}

/**
* 입력된 한 줄을 파싱하는 함수입니다. 오류가 있으면 1을 반환합니다.
*/
int parse_string(int line_index)
{
	char * copied_string = NULL; //strsep함수를 사용하기위해서는 내용이 변해도 문제없는 문자열이 필요합니다. 이를 위해 strdup함수를 이용해서 문자열을 복제할 것입니다.
	char * seperated_string = NULL; //strsep함수로 분리된 문자열은 여기에 저장됩니다. 이후에 값을 parse_str_array의 원소에 저장할 것입니다.
	char * delimiter = " "; //구분자 변수입니다.
	
	int check_result = 0; //파싱후의 구문을 검사한 결과 값입니다.
	
	correct_process_many = 0;
	
	copied_string = strdup(input_string_array[line_index]);
	remove_string_space(copied_string);

	//형식에 맞는지 검사합니다. 형식은 ' '가 3개 있는 형태여야합니다.
	if (letter_cnt(input_string_array[line_index],*delimiter) != 3)
	{
		fprintf(stderr,"invalid format in line %d, ignored\n",line_index + 1);
	}
	
	seperated_string = strsep(&copied_string, delimiter);
	remove_string_space(seperated_string);
	if ((check_result = check_id(seperated_string))) //먼저 id의 형식이 맞는지 확인합니다. 문제가 있으면 if는 참입니다.
	{
		if (check_result == 1)
			fprintf(stderr,"invalid process id ‘%s’ in line %d, ignored\n",seperated_string, line_index + 1);
		else if (check_result == 2)
			fprintf(stderr,"duplicate process id ‘%s’ in line %d, ignored\n",seperated_string,line_index + 1);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return 1;
	}
	
	//문제가 없으면 id를 구조체에 저장합니다.
	strcpy(parsed_str_array[line_index].program_id,seperated_string);
	
	seperated_string = strsep(&copied_string,delimiter);
	remove_string_space(seperated_string);
	
	if (check_arrive_time(seperated_string))
	{
		fprintf(stderr,"invalid arrive-time ‘%s’ in line %d, ignored\n",seperated_string, line_index + 1);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return 1;
	}
	
	parsed_str_array[line_index].arrive_time = atoi(seperated_string);
	
	seperated_string = strsep(&copied_string,delimiter);
	remove_string_space(seperated_string);
	if (check_service_time(seperated_string))
	{
		fprintf(stderr,"invalid service-time ‘%s’ in line %d, ignored\n",seperated_string, line_index + 1);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return 1;
	}
	
	parsed_str_array[line_index].service_time = atoi(seperated_string);
	
	seperated_string = strsep(&copied_string,delimiter);
	remove_string_space(seperated_string);
	if (check_priority(seperated_string))
	{
		fprintf(stderr,"invalid priority ‘%s’ in line %d, ignored\n",seperated_string, line_index + 1);
		parsed_str_array[line_index].program_id[0] = '#'; //오류가 있는 칸임을 알리는 값으로 id의 맨 앞을 #으로 지정합니다.
		return 1;
	}
	
	parsed_str_array[line_index].priority = atoi(seperated_string);
	
	
	
	correct_process_many++;
	last_process_index = line_index;
	
	
	printf("%s\t",parsed_str_array[line_index].program_id);
	printf("%d\t",parsed_str_array[line_index].arrive_time);
	printf("%d\t",parsed_str_array[line_index].service_time);
	printf("%d\n",parsed_str_array[line_index].priority);
	
	return 0;
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
