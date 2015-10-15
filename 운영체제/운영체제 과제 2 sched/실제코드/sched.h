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

const int SJF = 1;
const int SRT = 2;
const int RR = 3;
const int PR = 4;

FILE *argv_file; //data파일을 접근할 때 쓸 FILE 포인터입니다.
int line_many; //파일의 줄 수입니다. 이 수만큼 parsed_string과 process_running 배열의 길이를 할당합니다.
int correct_process_many; //형식에 맞는 프로세스의 갯수입니다.
int end_process_many; //현재까지 완료한 프로세스의 갯수입니다. 이 값이 correct_process_many와 같으면 모든 프로세스를 실행한 것이므로 검사를 종료합니다.
int last_process_index; //마지막으로 발견된 형식에 맞는 프로세스의 line_index입니다. 라인 인덱스를 이용하는 검사는 여기까지 검사합니다.

char** input_string_array; //파일 내부의 내용을 한 줄씩 나눠서 저장한 배열입니다.


/**
 * letter_list의 각 노드 타입이 될 구조체타입입니다.
 * char letter = 출력할 한 글자입니다.
 * struct letter_list* next_node = 다음에 출력할 글자를 가지고 있는 노드의 포인터입니다.
 */
typedef struct letter_node{
	char letter;
	struct letter_node* next_node;
}letter_node;

/**
 * 모든 계산이 끝난후에 실행 과정을 출력할 때 사용할 연결 리스트 구조체입니다.
 * int list_many = list에 있는 노드 수 = 길이입니다. 이 값이 0이 될때까지 출력하는 것이 알고리즘 평가의 마지막 과정입니다.
 * letter_node* head = list의 헤드(맨 앞의 노드)입니다.
 * letter_node* tail = list의 테일(맨 끝의 노드)입니다.
 */
typedef struct letter_list{
	int list_many;
	letter_node* head;
	letter_node* tail;

}letter_list;

/*
 * 파일내부 한줄에서 얻은 정보를 파싱해서 저장한 구조체입니다.
 * char program_id = 프로그램의 id입니다.
 * int arrive_time = 도착 시간(Context change overhead)입니다. 0 이상 30 이하여야합니다.
 * int service_time = 서비스 시간입니다. 1이상 30 이하여야합니다.
 */
typedef struct parsed_string{

	char program_id[3];
	int arrive_time;
	int service_time;
	int priority;
	
	int time_quntum;

	int remain_time;
	int complete_time;

	letter_list print_list;

}parsed_string;

int current_cpu_time; //CPU의 시계입니다. 특정 시간에 타이머 인터럽트가 걸리는 점을 표현하기 위해, 각 수행 직후=각 프로세스에 실행 내역을 저장한 후에 +1 되고 parsed_string을 탐색하면서 새로운 프로그램이 도착하는지 검사합니다.

int* ready_que; //Ready Que입니다. 원소는  길이는 correct_process_many입니다.
int ready_que_many; //현재 Ready Que의 길이입니다.
int cur_run_proc; //현재 실행 중인 프로세스의 ready_que 인덱스 입니다. -1은 현재 실행 중인 프로그램이 없다는 뜻입니다.

parsed_string* parsed_str_array; //각 라인의 파싱결과가 담긴 구조체들이 저장될 배열입니다.


//------------여기서 부터는 함수 선언만 있습니다.--------------//

int letter_cnt(char * string, char letter);
int check_arrive_time(char* seperated_string);
int check_id(char* seperated_string);
int check_priority(char* seperated_string);
int check_service_time(char* seperated_string);
void file_open(char** argv);
int parse_string(int line_index);
void read_data_file();
void read_new_line_letter();
void remove_string_space(char* string);
