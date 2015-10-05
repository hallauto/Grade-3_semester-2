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



//------------여기서 부터는 함수 선언만 있습니다.--------------//

typedef struct parsed_string{

	int arrive_time;
	int serviece_time;

}parsed_string;

void file_open(char** argv);
void read_data_file();