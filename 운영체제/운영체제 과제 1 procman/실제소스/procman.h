#include <stdio.h>
#include <string.h>

//한 줄의 길이는 최대 1000바이트입니다.
#define MAX_LENGTH 1000

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

/*
설정파일의 각 라인과, 라인의 내용들을 전부 구조체 배열로 만들어서 저장한 구조체입니다.
int line_many 실행해야할 행들의 수입니다. 이 값이 0이하면 프로세스 실행없이 바로 종료합니다.
char** input_string 각각의 행 문자열들을 저장한 변수입니다. malloc과 strdup를 활용해서 2차원 문자열 배열을 만듭니다.
*/
typedef struct  input_config
{
	int line_many;
	char** input_string;

} input_config;

input_config * struct_config;

/*
설정파일들의 각 라인을 파싱하고 그 내용을 이 구조체에 저장합니다. 각각의 구조체들은 배열로 보관됩니다.
int line_num 해당 라인의 번호입니다. 이 값이 구조체 배열에서의 인덱스가 됩니다.
int error_num 에러 여부를 알려주는 멤버변수입니다. 오류가 없다면 0이 ,오류가 있다면, 그 라인은 에러 번호를 0 이상을 주고 그외의 값은 넣지않습니다.
*/
typedef struct passed_string
{
	int line_num;
	int error_num;

	char* id;
	char action[10];
	char* pipe_id;
	char* command;

} passed_string;

passed_string * pass_str_array; //파싱된 후, 내용이 저장된 구조체를 여기에 저장합니다. 접근은 줄번호로 접근합니다.

int current_line; //현재 실행중인 행입니다. 이 값이 위의 input_setting구조체 변수의 배열의 인덱스로 쓰입니다.


/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


void read_config_file();
void read_new_line_letter();
void file_open(char **argv);
