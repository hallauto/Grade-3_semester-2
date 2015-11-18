
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int pid_many;
int* pid_array;

char* popen_result(FILE * fp)
{
	size_t readSize;
	char * popen_string = calloc(10240,sizeof(char)); //결과 문자열을 저장할 변수입니다.

    if( !fp)
    {
        printf("error [%d:%s]\n", errno, strerror(errno));
        return NULL;
    }

	//이제 결과 값을 읽습니다.
    readSize = fread( (void*)popen_string, sizeof(char), 10240-1, fp );

    pclose(fp);
    popen_string[readSize]=0;

    // print result
    printf("%s\n", popen_string);
    return popen_string;
}

int get_proc_pid()
{
	FILE * fp = NULL; //popen의 결과값을 얻기 위한 FILE 포인터입니다.
	char * pid_many_string;
	char * pid_string;
	char * seperated_pid; //proc에서 읽은 모든 pid를 파싱해서 나온 한개의 pid 문자열입니다. 이를 int형으로 바꿔서 pid_array에 저장할 것입니다.
	char * delim = "\n"; //전체 pid 문자열을 파싱할 구분자입니다.
	
	//먼저 pid의 갯수를 구합니다. 
	fp = popen("ls /proc | grep -E ^[0-9] | wc -l","r");
	pid_many_string = popen_result(fp);
	pid_many = atoi(pid_many_string) - 1; //기본적으로  위의 명령어의 결과 뒤에 가독성을 위한 \n이 붙습니다. 이를 감안해서 -1을 합니다.
	free(pid_many_string);
	
	fp = popen("ls /proc | grep -E ^[0-9]","r");
	pid_string = popen_result(fp);
	int pid_string_len = strlen(pid_string);
	
	pid_array = calloc(pid_many,sizeof(int)); //이제 pid갯수만큼 배열의 길이를 잡습니다.
	
	seperated_pid = strtok(pid_string,delim);
	if (seperated_pid != NULL && strcmp(seperated_pid, "") != 0)
	{
		printf("seperated_string[0] = %s\n",  seperated_pid);
		pid_array[0] = atoi(seperated_pid);
	}
	
	int pid_index; //pid를 탐색할 때 사용할 인덱스입니다.
	for (pid_index = 1; pid_index < pid_many; pid_index++)
	{
		seperated_pid = strtok(NULL,delim);
		if (seperated_pid != NULL && strcmp(seperated_pid, "") != 0)
		{
			printf("seperated_string[%d] = %s\n", pid_index, seperated_pid);
			pid_array[pid_index] = atoi(seperated_pid);
		}
	}
}

int main(void)
{
	int index;
	get_proc_pid();
	for (index = 0; index < pid_many; index++)
	{
		printf("pid_array[%d] = %d\n",index,pid_array[index]);
	}
}
