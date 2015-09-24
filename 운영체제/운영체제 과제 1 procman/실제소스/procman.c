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
  * 문자열의 앞뒤 공백을 제거하는 함수입니다.
  * 어디까지나 앞과 뒤만 제거하므로, 사이에 있는 공백은 제거하지 않습니다.
  * char* string: 공백을 제거할 문자열입니다.
  */
 int remove_string_space(char* string)
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
  * 아이디의 형식을 체크하는 함수입니다. 아이디는 2자~6자의 영어 소문자, 숫자만 사용 가능하며, 중복은 허용되지않습니다. 문자열이 공백인지도 여기에서 확인합니다.
  * 
  * char* string 검사할 아이디 문자열입니다.
  * 
  * return result 코드별로 뜻이 존재합니다.
  * 0 = 아이디의 형식이 잘못되었습니다.
  * 1 = 문제없습니다. 정상적으로 다음 단계로 넘어갑니다.
  */
 int check_id(char * string)
 {
	 int arscii_a = 97;
	 int arscii_z = 122;
	 int arscii_0 = 48;
	 int arscii_9 = 57;
	 
	 int string_index = 0; //문자열을 탐색할 인덱스입니다.
	 int string_length = strlen(string);
	 
	 if (string_length < 1) //문자열이 없습니다. 즉, 공백문자열과 같다고 할 수 있습니다.
	 {
		 return 1;
	 }
	 
	 //먼저 문자열의 앞뒤에서 공백을 제거합니다.
	 remove_string_space(string);
	
	//이제 문자열이 형식에 맞는지 검사합니다.
	for (string_index = 0; string_index < string_length; string_index++)
	{
		if (string[string_index]< arscii_a || string[string_index] > arscii_z)
		{
			if (string[string_index] < arscii_0 || string[string_index] > arscii_9)
				return 0;
		}
	}
	
	return 2;
 }
 
/**
 * 실제로 파싱을 하는 함수입니다. 한 줄을 읽으면 한번 호출 됩니다. 
 * 
 * int line_index: 파싱할 줄의 번호가 곧 input_string_array와 parse_str_array의 인덱스가 됩니다.
 * 
 * 파싱은 다음 순서대로 이루어집니다.
 * 1.구분자 갯수 확인(strcnt함수 이용)
 * 2.각각의 부분을 파싱(strsep이용), 임시 변수에 저장
 * 3.이 내용을 parse_str_array에 저장
 * 4.마지막으로 오류 검사
 * 
 */
int parse_command(int line_index)
{
	char * copied_string = NULL; //strsep함수를 사용하기위해서는 내용이 변해도 문제없는 문자열이 필요합니다. 이를 위해 strdup함수를 이용해서 문자열을 복제할 것입니다.
	char * seperated_string = NULL; //strsep함수로 분리된 문자열은 여기에 저장됩니다. 이후에 값을 parse_str_array의 원소에 저장할 것입니다.
	char * delimiter = ":"; //구분자 변수입니다.
	
	//먼저 형식에 맞는지 확인합니다. 기본적으로 커맨드는 ':' 문자가 3개 존재해야합니다.
	if (letter_cnt(input_string_array[line_index],*delimiter) != 3)
	{
		return 0; //오류가 있습니다. 다른 문제가 있는지 검사해야합니다.
	}
	
	copied_string = strdup(input_string_array[line_index]);
	
	seperated_string = strsep(&copied_string, delimiter);
	printf("id:%s	\n",seperated_string);
	if (!check_id(seperated_string)) //먼저 아이디부분을 확인합니다. 아이디는 2~6자의 영어,숫자로만 이루어져야합니다.
	{
		printf("이거 오류가 있는 커맨드입니다. 작동하면 안되요 안되\n");
	}
	else
	{
		
	}

	seperated_string = strsep(&copied_string, delimiter);
	//printf("action:%s	",seperated_string);
	
	seperated_string = strsep(&copied_string, delimiter);
	//printf("pipe-id:%s	길이=%d",seperated_string,strlen(seperated_string));

	seperated_string = strsep(&copied_string, delimiter);
	//printf("command:%s",seperated_string);
	
	return 1;
}


/**
 * config파일을 읽고 파싱과 적절한 구조체 생성을 지시하는 함수입니다.
 */
void read_config_file()
{
	size_t size = 0; //getline함수를 쓰기위해서 존재하는 변수입니다. 이 안에 값이 쓰일 일은 없습니다.
	read_new_line_letter();	//이 함수로 config파일의 줄 수를 확인하고 저장합니다.
	input_string_array = calloc(line_many,sizeof(char *));
	parse_str_array = calloc(line_many,sizeof(parsed_string *));

	int line_index;
	for (line_index = 0; !feof(argv_file); line_index++) //한줄씩 읽고 파싱합니다.
	{
		getline(&input_string_array[line_index],&size,argv_file);
		if (input_string_array[line_index][0] != '#' && input_string_array[line_index][0] != '\n' && strlen(input_string_array[line_index]) > 0)
		{
			parse_command(line_index);
			
		}
		else
		{
			printf("필요가 없는 줄입니다. --------- %s",input_string_array[line_index]);
		}
	}
	
	return;
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
