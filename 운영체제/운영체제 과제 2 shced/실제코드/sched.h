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

FILE *argv_file; //data파일을 접근할 때 쓸 FILE 포인터입니다.
int line_many; //파일의 줄 수입니다. 이 수만큼 parsed_string과 process_running 배열의 길이를 할당합니다.
int correct_process_many; //형식에 맞는 프로세스의 갯수입니다. 만약 이 값이 260개 이상이면, 이 이상의 프로세스는 계산에 쓰이지 않습니다.
int last_process_index; //마지막으로 발견된 형식에 맞는 프로세스의 line_index입니다. 라인인덱스를 이용하는 검사는 여기까지 검사합니다.

char** input_string_array; //파일 내부의 내용을 한 줄씩 나눠서 저장한 배열입니다.

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

}parsed_string;

int total_cpu_time; //각 프로그램의 service타임을 합산한 값으로, 이후 출력될 프로그램의 실행도의 길이가 됩니다.

/*
 * 실행중인 프로세스에 관한 정보를 저장한 구조체입니다.
 * char process_id[3]: 실행중인 해당 프로세스의 id입니다.
 * int line_index: 해당 프로세스가 적혀져있던 줄 번호입니다. 이를 이용해서 parsed_str_array에 접근할 수 있습니다.
 * int remain_time: 일정 시간 실행 후, 현재 남아있는 시간입니다. 이 값이 0이하가 되면 이 프로세스는 작업을 끝낸것이므로 이후 스케쥴러는 이 프로세스의 처리는 무시합니다.
 * int complete_time: 최종적으로 종료된 시간입니다. 
 */
typedef struct process_running{

	char process_id[3];
	int line_index;
	int remain_time;
	int complete_time;

}process_running;

parsed_string* parsed_str_array; //각 라인의 파싱결과가 담긴 구조체들이 저장될 배열입니다.
process_running* proc_run_array; //실행 중인 프로세스의 정보가 담긴 구조체들이 저장될 배열입니다.


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
