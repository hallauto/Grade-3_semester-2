#include <stdio.h>
#include <string.h>

//에러를 확인할 때 쓰는 코드입니다. 이후 결과 파일에 출력할 때, 이 값과 해당 라인의 에러 번호를 비교해서 검사합니다.
const int NONE_ERROR = 0;
const int ERROR_NO_SELECT = 1;
const int ERROR_WRONG_SELECT = 2;
const int ERROR_INVALID_FORMAT = 3;
const int ERROR_ID_ERROR = 4;
const int ERROR_DUPLICATE_ID = 5;
const int ERROR_WRONG_ACTION = 6;
const int ERROR_NO_PIPE_ID = 7;
const int ERROR_RESPAWN_WITH_PIPE = 8;
const int ERROR_EXIST_PIPE = 9;
const int ERROR_EMPTY_LINE = 10;
const int ERROR_FAIL_EXECUTE = 11;

//액션에 해당하는 값들을 미리 const로 초기화합니다. 이들을 입력된 문자열과 비교합니다.
const char ACTION_ONCE[10] = "once";
const char ACTION_WAIT[10] = "wait";
const char ACTION_RESPAWN[10] = "respawn";

//config 파일을 가리키는 FILE 포인터입니다. 
FILE* argv_file;		

//int line_many 실행해야할 행들의 수입니다. 이 값이 0이하면 프로세스 실행없이 바로 종료합니다.
int line_many;
//char** input_string 각각의 행 문자열들을 저장한 변수입니다. malloc과 strdup를 활용해서 2차원 문자열 배열을 만듭니다.
char** input_string_array;

/*
설정파일들의 각 라인을 파싱하고 그 내용을 이 구조체에 저장합니다. 각각의 구조체들은 배열로 보관됩니다.
int line_num 해당 라인의 번호입니다. 이 값이 구조체 배열에서의 인덱스가 됩니다.
char* id 해당 프로그램의 아이디입니다.
char action[10]
char pipe_id
char command

*/
typedef struct parsed_string
{
	int line_num;

	char* id;
	char action[10];
	char* pipe_id;
	char* command;

} parsed_string;

parsed_string ** parse_str_array; //파싱된 후, 내용이 저장된 구조체를 여기에 저장합니다. 접근은 줄번호로 접근합니다.

char ** pipe_id_array; //파이프로 연결된 아이디들의 배열입니다. 배열의 길이는 line_many*2입니다.
int pipe_many; //파이프로 연결된 아이디 갯수입니다.

/**
 * 실행중인 프로세스를 나타내는 구조체입니다.
 * int process_id 해당 프로세스의 프로세스 아이디입니다.
 * char* program_id config파일에서 언급된 프로그램 아이디입니다.
 * char* action 설정된 액션입니다.
 */
typedef struct process_running
{
	int process_id;
	char * program_id;
	
	char action[10];
	
}process_running;

process_running ** proc_array; //실행 중인 프로세스에 관한 정보가 담긴 구조체 배열입니다. 접근은 줄번호로 접근합니다.

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
//함수 선언을 하는 구간입니다. 자세한 설명은 각각의 함수 정의 부분에 있습니다.


int check_action(char* string);
int check_id(char * string);
int check_pipe_id(char* string);
void file_open(char **argv);
int letter_cnt(char * string, char letter);
char** parse_command(int line_index);
int parse_config_string(int line_index);
void process_run();
void read_config_file();
void read_new_line_letter();
void remove_string_space(char* string);
void signal_regist();
