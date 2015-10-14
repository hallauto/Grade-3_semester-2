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
int correct_process_many; //형식에 맞는 프로세스의 갯수입니다. 만약 이 값이 260개 이상이면, 이 이상의 프로세스는 계산에 쓰이지 않습니다.
int last_process_index; //마지막으로 발견된 형식에 맞는 프로세스의 line_index입니다. 라인인덱스를 이용하는 검사는 여기까지 검사합니다.

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

	int remain_time;
	int complete_time;

	letter_list print_list;

}parsed_string;

int current_cpu_time; //CPU의 시계입니다. 특정 시간에 타이머 인터럽트가 걸리는 점을 표현하기 위해, 각 수행 직후=각 프로세스에 실행 내역을 저장한 후에 +1 되고 parsed_string을 탐색하면서 새로운 프로그램이 도착하는지 검사합니다.

/*
이 녀석 필요 없을지도...
/*
 * 실행하고 스케쥴링하게될 프로세스에 관한 정보를 저장한 구조체입니다. 지난번 과제와의 차이점은 이 구조체의 배열은 단순히 실행중인 프로세스 뿐만 아니라, 실행 된 후에 종료된 프로세스들의 정보도 그대로 유지된다는 것입니다.
 * char process_id[3]: 실행중인 해당 프로세스의 id입니다.
 * int line_index: 해당 프로세스가 적혀져있던 줄 번호입니다. 이를 이용해서 parsed_str_array에 접근할 수 있습니다.
 * int remain_time: 일정 시간 실행 후, 현재 남아있는 시간입니다. 이 값이 0이하가 되면 이 프로세스는 작업을 끝낸것이므로 이후 스케쥴러는 이 프로세스의 처리는 무시합니다.
 * int complete_time: 최종적으로 종료된 시간입니다. 
 
typedef struct process_correct{

	char process_id[3];
	int line_index;

}process_correct;

*/

int* running_que; //Running Que입니다. 길이는 correct_process_many입니다.
int run_que_many; //현재 Running Que의 길이입니다.
int cur_run_proc; //현재 실행중인 프로세스입니다.

parsed_string* parsed_str_array; //각 라인의 파싱결과가 담긴 구조체들이 저장될 배열입니다.
//process_running* proc_run_array; //실행 중인 프로세스의 정보가 담긴 구조체들이 저장될 배열입니다.


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
