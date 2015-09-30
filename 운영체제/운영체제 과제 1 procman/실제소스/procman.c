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
		fprintf(stderr,"failed to load config file ‘%s’\n",file_name);
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
  * 아이디의 형식을 체크하는 함수입니다. 아이디는 2자~6자의 영어 소문자, 숫자만 사용 가능하며, 중복은 허용되지않습니다. 문자열이 공백인지도 여기에서 확인합니다.
  * 
  * char* string 검사할 아이디 문자열입니다.
  * 
  * return result 코드별로 뜻이 존재합니다.
  * 0 = 문제없습니다. 정상적으로 다음 단계로 넘어갑니다.
  * 1 = 아이디의 형식이 잘못되었습니다.
  * 2 = 중복되는 아이디가 존재합니다.
  * 3 = 이미 파이프에 연결된 아이디입니다.
  */
 int check_id(char * string)
 {
	 int arscii_a = 97;
	 int arscii_z = 122;
	 int arscii_0 = 48;
	 int arscii_9 = 57;
	 
	 int string_index = 0; //문자열을 탐색할 인덱스입니다.
	 int string_length = 0; //문자열의 길이입니다.
	 
	 //먼저 문자열의 앞뒤에서 공백을 제거합니다.
	 remove_string_space(string);
	 
	 string_length = strlen(string);
	 if (string_length < 2 || string_length > 8)
	 {
		 return 1;
	 }
	
	//이제 문자열이 형식에 맞는지 검사합니다.
	for (string_index = 0; string_index < string_length; string_index++)
	{
		if (string[string_index]< arscii_a || string[string_index] > arscii_z)
		{
			if (string[string_index] < arscii_0 || string[string_index] > arscii_9)
				return 1;
		}
	}
	
	//마지막으로 중복이 없는지 검사합니다.
	int array_index; //parsed_array를 탐색하는데에 쓰일 배열입니다.
	for (array_index = 0; array_index < line_many; array_index++)
	{
		if (parse_str_array[array_index] == NULL) //parse_str_array에서 정상적인 커맨드가 파싱되어 저장되지 않은 곳은 전부 NULL값입니다. 당연히 탐색할 필요가 없습니다.
			continue;
		else if(strcmp(parse_str_array[array_index]->id,string)==0) //이제 id값이 같은 구조체가 존재하는지 검사합니다. 존재한다면 2(오류)을 반환합니다.
		{
			return 2;
		}
	}
	
	return 0;
 }
 
 /**
  * action을 검사하는 함수입니다.
  * char * string: 검사할 문자열입니다.
  * return: int 오류 코드
  * 0: 오류 없음
  * 1: 존재하지않는 액션
  */
 int check_action(char* string)
 {
	 remove_string_space(string);
	 
	 if (strlen(string) < 1)
	 {
		 return 1;
	 }
	 
	 if (strcmp(string,ACTION_ONCE)==0)
	 {
		 return 0;
	 }
	 else if (strcmp(string,ACTION_RESPAWN)==0)
	 {
		 return 0;
	 }
	 else if (strcmp(string,ACTION_WAIT)==0)
	 {
		 return 0;
	 }
	 return 1;
 }
 
 /**
  * 파이프 아이디 형식 검사 및, 사용 가능 여부를 검사하는 함수입니다.
  * char * string: 검사할 문자열입니다.
  * return: int 오류 코드
  * 0: 오류 없음
  * 1: 아이디의 형식이 잘못되었습니다.
  * 2: 존재하지 않는 아이디입니다.
  * 3: 이미 다른 프로세스와 파이프로 연결되어 있는 아이디입니다.
  */
 int check_pipe_id(char* string)
 {
	 int arscii_a = 97;
	 int arscii_z = 122;
	 int arscii_0 = 48;
	 int arscii_9 = 57;
	 
	 int string_index = 0;
	 int string_length = 0;
	 
	 //int id_exist = 0; // 해당 아이디가 존재하는지 나타내는 변수입니다.
	 
	 remove_string_space(string);
	 string_length = strlen(string);
	 
	 if (string_length < 1) //파이프가 지정되어있지 않습니다.
	 {
		 return 0;
	 }
	 else if(string_length < 2 || string_length > 8)
	 {
		 return 1;
	 }
	 	
	//이제 문자열이 형식에 맞는지 검사합니다.
	for (string_index = 0; string_index < string_length; string_index++)
	{
		if (string[string_index]< arscii_a || string[string_index] > arscii_z)
		{
			if (string[string_index] < arscii_0 || string[string_index] > arscii_9)
				return 1;
		}
	}
	
	//이미 파이프로 연결된 프로세스를 이용하는지 검사합니다.
	int pipe_array_index; //pipe_id_array를 탐색하는데에 쓰일 배열입니다.
	for (pipe_array_index = 0; pipe_array_index < pipe_many ; pipe_array_index++)
	{
		if (strcmp(string,pipe_id_array[pipe_array_index]) == 0)
		{
			return 3;
		}
	}
	
	//마지막으로 해당 프로세스가 있는지, 있다면 사용 가능한지 검사합니다.
	int array_index; //parsed_array를 탐색하는데에 쓰일 배열입니다.
	for (array_index = 0; array_index < line_many; array_index++)
	{
		if (parse_str_array[array_index] == NULL) //parse_str_array에서 정상적인 커맨드가 파싱되어 저장되지 않은 곳은 전부 NULL값입니다. 당연히 탐색할 필요가 없습니다.
			continue;
		else if(strcmp(parse_str_array[array_index]->id,string)==0) //이제 id값이 같은 구조체가 존재하는지 검사합니다.
		{
			return 0;
		}
	}
	return 2; //해당하는 아이디를 가진 프로세스는 없습니다.
 }
 
/**
 * 실제로 파싱을 하는 함수입니다. 한 줄을 읽으면 한번 호출 됩니다. 
 * 
 * int line_index: 파싱할 줄의 번호가 곧 input_string_array와 parse_str_array의 인덱스가 됩니다.
 * 
 * return int 오류 번호: 파싱중에 생긴 오류에 따라 번호가 반환됩니다. 이 오류를 구분해서 stderr에 출력합니다.
 * 0: 오류 없음
 * 1: 형식에 맞지 않습니다. 잘못된 config파일입니다.
 * 2: 파싱후 확인한 결과, 오류가 있습니다. 오류 내용은 출력했으므로 다음 줄로 넘어가야합니다.
 * 
 * 파싱은 다음 순서대로 이루어집니다.
 * 1.구분자 갯수 확인(strcnt함수 이용)
 * 2.각각의 부분을 파싱(strsep이용), 임시 변수에 저장
 * 3.이 내용을 parse_str_array에 저장
 * 4.마지막으로 오류 검사
 * 
 */
int parse_config_string(int line_index)
{
	char * copied_string = NULL; //strsep함수를 사용하기위해서는 내용이 변해도 문제없는 문자열이 필요합니다. 이를 위해 strdup함수를 이용해서 문자열을 복제할 것입니다.
	char * seperated_string = NULL; //strsep함수로 분리된 문자열은 여기에 저장됩니다. 이후에 값을 parse_str_array의 원소에 저장할 것입니다.
	char * delimiter = ":"; //구분자 변수입니다.
	
	parsed_string* parsed_struct = calloc(1,sizeof(parsed_string)); //parsed_string에 저장될 원소입니다. 만약 커맨드에 오류가 있다면 free로 메모리를 없애고 parsed_array에는 아무 값도 저장되지 않습니다.
	
	int check_result = 0; //입력된 값들이 조건을 만족하는지 검사한 후의 결과 값을 저장할 변수입니다.
	
	//먼저 형식에 맞는지 확인합니다. 기본적으로 커맨드는 ':' 문자가 3개 존재해야합니다.
	if (letter_cnt(input_string_array[line_index],*delimiter) != 3)
	{
		free(parsed_struct);
		printf("invalid format in line %d, ignored\n",line_index + 1);
		return 1; //오류가 있습니다. 다른 문제가 있는지 검사해야합니다.
	}
	
	copied_string = strdup(input_string_array[line_index]);
	
	seperated_string = strsep(&copied_string, delimiter);
	if ((check_result = check_id(seperated_string))) //먼저 아이디부분을 확인합니다. 아이디는 2~6자의 영어,숫자로만 이루어져야합니다. 또한 중복이 없어야합니다.
	{
		free(parsed_struct);
		if (check_result == 1)
		{
			printf("invalid id ‘%s’ in line %d, ignored\n", seperated_string, line_index + 1);
		}
		else if (check_result == 2)
		{
			printf("duplicate id ‘%s’ in line %d, ignored\n", seperated_string, line_index + 1);
		}
		return 1;
	}
	else
	{
		parsed_struct->id = strdup(seperated_string);
	}

	//이번에는 action 값을 파싱으로 가져옵니다.
	seperated_string = strsep(&copied_string, delimiter);
	if ((check_result = check_action(seperated_string))) //action에 오류가 있는지 검사합니다.
	{
		free(parsed_struct->id);
		free(parsed_struct);
		if (check_result == 1)
		{
			printf("invalid action ‘%s’ in line %d, ignored\n",seperated_string,line_index + 1);
		}
		return 1;
	}
	else
	{
		strcpy(parsed_struct->action,seperated_string);
	}
	
	seperated_string = strsep(&copied_string, delimiter);
	if((check_result = check_pipe_id(seperated_string)))
	{
		free(parsed_struct->id);
		free(parsed_struct);
		if (check_result == 1 && strcmp(seperated_string,""))
		{
			printf("invalid pipe-id ‘%s’ in line %d, ignored\n", seperated_string, line_index + 1);
		}
		else if (check_result == 2)
		{
			printf("unknown pipe-id ‘%s’ in line %d, ignored\n",seperated_string, line_index + 1);
		}
		else if (check_result == 3)
		{
			printf("pipe not allowed for already piped tasks in line %d, ignored\n", line_index + 1);
		}
		
		return 1;
	}
	else if (strcmp(seperated_string,"") != 0)
	{
		if (strcmp(parsed_struct->action,ACTION_RESPAWN)==0) //check_pipe_id와 별개로, pipe_id가 지정된 상태에서 action이 respawn인지 검사합니다.  맞다면 오류입니다.
		{
			free(parsed_struct->id);
			free(parsed_struct);
			printf("pipe not allowed for 'respawn' tasks in line %d, ignored\n",line_index + 1);
			return 1;
		}
		else
		{
			parsed_struct->pipe_id = strdup(seperated_string);
		}
	}
	else
	{
		parsed_struct->pipe_id = strdup(seperated_string);
	}

	seperated_string = strsep(&copied_string, delimiter);
	remove_string_space(seperated_string);
	if (strcmp(seperated_string,"") == 0) //커맨드 부분이 공백인지 확인합니다. 만약 공백이라면 오류 메시지를 전송합니다. 만약 파이프 연결이 등록되어 있었다면 이 사항도 취소합니다.
	{
		printf("empty command in line %d, ignored\n", line_index + 1);
		return 1;
	}
	else //이 줄은 정상적으로 작성되었습니다. 이제 파이프로 연결되었다는 사항과 기타 내용을 등록합니다.
	{
		if (strcmp(parsed_struct->pipe_id,"") != 0)
		{
			pipe_id_array[pipe_many++] = strdup(parsed_struct->id);
			pipe_id_array[pipe_many++] = strdup(parsed_struct->pipe_id);
		}
		parsed_struct->command = strdup(seperated_string);
		parse_str_array[line_index] = parsed_struct;
	}
	
	return 0;
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
	pipe_id_array = calloc(line_many * 2,sizeof(char *));
	pipe_many = 0;

	int line_index;
	for (line_index = 0; !feof(argv_file); line_index++) //한줄씩 읽고 파싱합니다.
	{
		getline(&input_string_array[line_index],&size,argv_file);
		if (input_string_array[line_index][0] != '#' && input_string_array[line_index][0] != '\n' && strlen(input_string_array[line_index]) > 0)
		{
			parse_config_string(line_index);
			
		}
	}
	
	return;
}

void connect_pipe()
{
	
}

/**
 * 부모 프로세스의 SIGCHLD 핸들러입니다. 자식 프로세스가 종료되면 SIGCHLD가 전달되고, 이 때에 waitpid함수로 자식 프로세스의 종료를 확인합니다. 만약 프로세스가 respawn이라면 다시 시작 시킵니다.
 * int signo: 전달된 시그널의 번호입니다.
 */
void sigchld_handler_parrents(int signo)
{

}

/**
 * 부모 프로세스의 SIGINT 핸들러입니다. 이 함수가 전달되면 부모 프로세스는 자식프로세스들에게 SIGINT를 전달하면서 프로세스가 전부 종료되었는지 검사합니다. 
 * 만약 종료되지 않은 프로세스가 있다면 sleep으로 좀 기다리다가 다시 한번 보내고 또 검사하고... 이를 반복합니다.
 * 참고로 respawn이고 뭐고 전부 다시 실행될 수 없도록 죽입니다.(예제의 Task4의 경우입니다.) 이것이 위의 SIGCHLD핸들러와의 차이점입니다.
 * int signo: 전달된 시그널의 번호입니다.
 */
void sigint_handler_parrents(int signo)
{
	
}

/**
 * 부모 프로세스의 SIGTERM 핸들러입니다. 이 함수가 전달되면 부모 프로세스는 자식프로세스들에게 SIGINT를 전달하면서 프로세스가 전부 종료되었는지 검사합니다. 
 * 만약 종료되지 않은 프로세스가 있다면 sleep으로 좀 기다리다가 다시 한번 보내고 또 검사하고... 이를 반복합니다.
 * int signo: 전달된 시그널의 번호입니다.
 */
void sigterm_handler_parrents(int signo)
{

}
/**
 * 시그널의 기본동작 대신 지정된 동작을 하도록 저장합니다.
 * 그 이전에 말하자면, 이 함수는 fork함수의 반환값으로 부모, 자식 프로세스를 구분합니다. 즉, 전달되는 pid가 0이 아니면 자식 프로세스의 핸들러, 맞다면 부모 프로세스 핸들러를 등록해야합니다.
 * int pid: 프로세스 아이디입니다.
 */
void signal_regist()
{
	
}

/**
 * 커맨드를 파싱하는 함수입니다. 결과는 char** 로 전달됩니다.(동적 문자열의 배열)
 * int line_index: 파싱할 커맨드가 있는 인덱스입니다. 이 인덱스는 parse_str_array의 인덱스입니다.
 * return parse_result: execv의 두번째 인자(인자 배열)이 될 '인수들의 배열'입니다. NULL인지 꼭 검사하세요. NULL이라면 오류가 있다는 것입니다.
 */
void parse_command(char* result[20], char* str)
{
	char * copied_string = NULL; //strsep함수를 사용하기위해서는 내용이 변해도 문제없는 문자열이 필요합니다. 이를 위해 strdup함수를 이용해서 문자열을 복제할 것입니다.
	char * delimiter = " "; //커맨드의 구분자는 스페이스 바입니다.
	char * seperated_string = NULL;
	int parameter_many; //인자의 갯수입니다. 이를 계산해서 문자열 배열 parse_result에 필요한 길이가 얼마인지 알아냅니다.

	//먼저 인자의 갯수가 몇개인지 알아봐야합니다. 이를 검색하기위해 strsep를 활용합니다.
	copied_string = strdup(str);

	for(parameter_many=0;(seperated_string = strsep(&copied_string, delimiter)) != NULL;parameter_many++)
	{
	}

	if (copied_string != NULL && strcmp(copied_string,"") == 0)
	{
		free(copied_string);
	}
	copied_string = strdup(str);

	for(parameter_many=0;(seperated_string = strsep(&copied_string, delimiter)) != NULL;parameter_many++)
	{
		result[parameter_many] = strdup(seperated_string);
		printf("%s\n",result[parameter_many]);
	}
}

/**
 * 한 줄의 프로그램을 수행합니다. 주어진 라인 인덱스로 parse_str_array에 접근하고, 여기에서 얻은 데이터로 프로세스를 실행합니다.
 */
void oneline_process_run(int line_index)
{
	char* seperated_string[20] = {0};
	int child_return = 0; //자식 프로세스가 종료 후 반환한 값에 대한 내용입니다.

	char* copied_string = strdup(parse_str_array[line_index]->command);
	
	parse_command(seperated_string,copied_string);
	if (copied_string != NULL && strcmp(copied_string,"") == 0)
	{
		free(copied_string);
	}
	
	process_running* new_proc = calloc(1,sizeof(process_running)); //새로운 프로세스의 관리를 위한 구조체를 만듭니다.
	new_proc->program_id = strdup(parse_str_array[line_index]->id); //기본적으로는 모든 프로세스에게 프로그램 아이디가 존재합니다. 이를 전달합니다.


	if (strcmp(parse_str_array[line_index]->action,ACTION_ONCE) == 0)
	{
		return;
		strcpy(new_proc->action,ACTION_ONCE);
		new_proc->process_id = fork();
	}
	else if (strcmp(parse_str_array[line_index]->action,ACTION_WAIT) == 0)
	{
		strcpy(new_proc->action,ACTION_WAIT);
		new_proc->process_id = fork();
		if (new_proc->process_id == 0)
		{
			//여기서 부터는 자식 프로세스의 영역입니다. 여기에서 exec를 작동시켜야합니다. 물론 그이전에 파이프 연결과 시그널 핸들러 등록도 이루어져야 합니다.
			if (strcmp(parse_str_array[line_index]->pipe_id,"") != 0) //파이프에 연결을 해야하는지 검사합니다.
			{
				printf("프로세스 %s는 파이프로 프로세스 %s와 연결됩니다.\n",parse_str_array[line_index]->id,parse_str_array[line_index]->pipe_id);
				connect_pipe(); //파이프에 연결합니다. 이 때, 파이프를 연결할 다른 한쪽의 프로세스의 줄 인덱스도 전달해야 합니다.
			}
			execv(seperated_string[0],seperated_string);
		}
		waitpid(new_proc->process_id,&child_return,0); //이제 해당 프로그램이 실행이 끝날때까지 기다립니다. 그것이 action wait입니다.
	}
	else if (strcmp(parse_str_array[line_index]->action,ACTION_RESPAWN) == 0)
	{
		return;
		strcpy(new_proc->action,ACTION_RESPAWN);
		new_proc->process_id = fork();
		if (new_proc->process_id == 0)
		{
			//여기서 부터는 자식 프로세스의 영역입니다. 여기에서 exec를 작동시켜야합니다. 물론 그이전에 파이프 연결과 시그널 핸들러 등록도 이루어져야 합니다.
			if (strcmp(parse_str_array[line_index]->pipe_id,"") != 0) //파이프에 연결을 해야하는지 검사합니다.
			{
				connect_pipe(); //파이프에 연결합니다. 이 때, 파이프를 연결할 다른 한쪽의 프로세스의 줄 인덱스도 전달해야 합니다.
			}
		}
	}
}

/**
 * 프로세스를 실행시키는 함수입니다. 여기에서 자식 프로세스를 만듭니다.
 */
void process_run()
{
	int line_index = 0; //현재 실행시킬 프로그램의 줄번호가 배열의 인덱스입니다.
	

	proc_array = calloc(line_many,sizeof(process_running *));

	
	for (line_index = 0; line_index < line_many; line_index++)
	{
		if (parse_str_array[line_index] == NULL)
			continue;
		else
		{
			oneline_process_run(line_index);
		}
	}
	
	//커맨드부분의 파싱이 끝났으니 이제 다음에 할 일은 프로세스의 액션에 맞춰 프로그램을 실행하는 것 뿐입니다.
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
		fprintf (stderr, "configuration file must specified\n");
		return -1;
	}
	else
	{
		file_open(argv);
		signal_regist();
		process_run();
	}
	return 0;
}
