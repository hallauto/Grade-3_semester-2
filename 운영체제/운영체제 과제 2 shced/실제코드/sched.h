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


typedef struct parsed_string{

	char program_name[3];
	int arrive_time;
	int service_time;
	int priority;

}parsed_string;

int total_cpu_time; //각 프로그램의 service타임을 합산한 값으로, 이후 출력될 프로그램의 실행도의 길이가 됩니다.

typedef struct process_running{

	char process_name[3];
	int remain_time;
	int change_count; //스케쥴러로 인해 Context_change가 일어난 횟수입니다. 이 횟수를 기반으로 이후 계산할 Average Turnaround Time, Average Wating Time을 계산합니다. 

}process_running;

parsed_string* parsed_str_array; //각 라인의 파싱결과가 담긴 구조체들이 저장될 배열입니다.
process_running* proc_run_array; //실행 중인 프로세스의 정보가 담긴 구조체들이 저장될 배열입니다.


//------------여기서 부터는 함수 선언만 있습니다.--------------//

void file_open(char** argv);
void read_data_file();