/*
 * OS Assignment #4
 *
 * @file pfs.c
 */

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

void get_proc_pid();
char* get_pid_proc_cmdline(int pid);
void get_pid_proc_file_path(int pid_index);
char* make_pid_proc_file_path(int pid_index);
char* popen_result(FILE * fp);
void get_status_file(int pid_index);

typedef struct proc_file{
	
	char proc_cmdline[201]; //해당 프로세스의 실행 명령어입니다.
	char proc_file_path[257]; //위의 두 변수를 합쳐서 만든 파일 이름입니다.
	int proc_owner; //해당 프로세스의 사용자 = 파일 소유자입니다.
	int proc_group; //해당 프로세스의 그룹 입니다.
	int vmsize; //해당 프로세스의 vmsize입니다.
	time_t create_proc_time; //해당 프로세스의 생성 시간입니다.
	time_t access_proc_time; //해당 프로세스의 최근 접근 시간입니다.
	time_t modifi_proc_time; //해당 프로세스의 수정된 시간입니다.
	
	int proc_error; //해당 pid의 프로세스에 문제가 있는지 없는지를 알려주는 값입니다. 만약 이 값이 1 이상이라면, 오류가 있는 것이므로 출력하지 않습니다.
	
}proc_file;

static proc_file proc_file_var;

static int pid_many; //전체 pid갯수입니다.
static int* pid_array; //pid를 저장할 배열입니다. 모든 pid들이 여기에 저장됩니다.

static int pfs_getattr (const char  *path,
                        struct stat *stbuf)
{
	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, "") != 0){
		stbuf->st_mode = S_IFREG | 644;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1000;
	}
	else
		res = -ENOENT;
		
	return res;
}


static int pfs_readdir (const char            *path,
                        void                  *buf,
                        fuse_fill_dir_t        filler,
                        off_t                  offset,
                        struct fuse_file_info *fi)
{
	int pid_index;
	(void) offset;
	(void) fi;	

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    
    for(pid_index = 0; pid_index < pid_many; pid_many++)
    {
		proc_file_var.proc_error = 0; //에러를 판단할 때 사용하는 값을 초기화합니다.
		
		get_pid_proc_file_path(pid_index);
		
		if (proc_file_var.proc_error > 0)
		{
			pid_index++;
			continue;
		}
		
		if (proc_file_var.proc_file_path != NULL && proc_file_var.proc_file_path[0] == '/')
		{
			filler(buf, proc_file_var.proc_file_path + 1, NULL, 0);
		}
	}
    
	return 0;
}

void get_pid_proc_file_path(int pid_index)
{
	
	char* proc_cmdline = NULL; //검색된 프로세스의 실행 명령어입니다.
	char* proc_file_path = NULL; //위의 두 변수를 합쳐서 만든 파일 이름입니다.
	
	if (pid_array[pid_index] < 1) //만약 pid가 1 미만이라면 이는 오류입니다. 이 프로세스는 무시하고 다른 프로세스를 탐색합니다.
	{
		return;
	}
	else //특별한 오류가 없으니 이제부터 해당 pid를 가진 프로세스의 정보를 모읍니다.
	{
		proc_cmdline = get_pid_proc_cmdline(pid_array[pid_index]);
		if (proc_cmdline == NULL || strcmp(proc_cmdline, "") == 0) //명령어가 없다는 것은 뭔가 오류가 있는 것입니다. 다음 단계로 넘어갑니다.
		{
			proc_file_var.proc_error = 2;
			if (proc_cmdline!=NULL)
			{
				proc_cmdline = NULL;
			}
			return;
		}
		else
		{
			if (strlen(proc_cmdline) > 200)
			{
				strncpy(proc_file_var.proc_cmdline,proc_cmdline,200);
			}
			else
			{
				strcpy(proc_file_var.proc_cmdline,proc_cmdline);
			}
			proc_cmdline = NULL;
			proc_file_path = make_pid_proc_file_path(pid_index);
		}
		if (proc_file_path == NULL || strcmp(proc_file_path,"") ==0)
		{
			proc_file_var.proc_error = 2;
			printf("proc_file_path 생성 오류 발생\n");
		}
		else
		{
			strcpy(proc_file_var.proc_file_path,proc_file_path);
			proc_file_path = NULL;
		}
	}
}
char* get_pid_proc_cmdline(int pid)
{
	char proc_path[1024]; //proc 내부의 파일의 경로를 저장할 배열입니다.
	char * cmdline_string = NULL; //cmdline을 읽고 얻은 결과값입니다. 이 값에서 /를 -로 고치고, 앞부분에 있는 -들을 제거해서 반환해야합니다.
	size_t size = -1; //getline함수를 통해 할당된 메모리의 크기입니다. 이 값이 -1 그대로 유지되었다면 메모리가 할당되지 않은 것입니다.
	ssize_t return_ssize = 0; //getline함수의 반환 값입니다.
	FILE * fp;
	sprintf(proc_path,"/proc/%d/cmdline",pid);
	fp = fopen(proc_path,"r");
	if (fp == NULL)
	{
		return NULL;
	}
	return_ssize = getline(&cmdline_string,&size,fp);
	if (size == -1)
	{
		cmdline_string = NULL;
	}
	if (return_ssize == -1)
	{
		cmdline_string = NULL;
	}
	
	return cmdline_string;
}

/*
  pid와 프로세스 이름을 합쳐서 파일 이름을 만듭니다.
*/
char* make_pid_proc_file_path(int pid_index)
{
	char* file_path_string = NULL; //반환될 결과 값, 파일 이름 문자열입니다.
	char copy_cmdline_string[201]; //변환될 명령어 문자열의 복사본입니다.
	char pid_string[57]; //문자열 타입의 pid를 저장할 변수입니다.
	int string_len; //명령어 문자열의 길이입니다.
	int string_index; //명령어 문자열을 탐색할 때 쓰일 인덱스입니다.
	int pid_len; //pid 문자열의 길이입니다.
	
	strcpy(copy_cmdline_string,proc_file_var.proc_cmdline);
	string_len = strlen(copy_cmdline_string);
	sprintf(pid_string,"%d",pid_array[pid_index]);
	pid_len = strlen(pid_string);
	
	//먼저 /를 전부 -로 변환합니다.
	for (string_index = 0; string_index < string_len; string_index++)
	{
		if (copy_cmdline_string[string_index] == '/')
			copy_cmdline_string[string_index] = '-';
	}
	
	//이제 앞에 있는-들을 전부 없앱니다. 이후에 pid와 연결될 때 -를 추가할 것입니다.
	while (copy_cmdline_string[0] == '-')
	{
		for (string_index = 1; string_index <= string_len; string_index++) //맨 앞 바로 뒤부터 이 뒤에 있는 값을 앞으로 넣어야합니다. 이를 위한 함수입니다.
		{
			copy_cmdline_string[string_index - 1] = copy_cmdline_string[string_index];
		}
		string_len--; //줄의 길이를 나타내는 변수를 1 줄입니다.
	}
	
	file_path_string = (char*)calloc(pid_len+string_len+2,sizeof(char)); //경로 문자열의 길이는 앞의 /와 뒤의 NULL을 포함해서 계산합니다.
	file_path_string[0] = '/'; //모든 경로의 시작은 /입니다.
	strcat(file_path_string,pid_string);
	strcat(file_path_string,"-");
	strcat(file_path_string,copy_cmdline_string);
	
	return file_path_string;
}


static int pfs_unlink (const char *path)
{
	/* not yet implemented */
	return -ENOENT;
}

/*
 * popen을 통해 얻은 출력값을 문자열로 반환하는 함수입니다.
 * FILE * fp: 명령어를 실행한 popen의 반환 값(파이프의 파일 디스크립터)입니다.
 */
char* popen_result(FILE * fp)
{
	size_t readSize;
	char * popen_string = NULL; //결과 문자열을 저장할 변수입니다.
	
	popen_string = (char*)malloc(10240*sizeof(char)); 

    if( !fp)
    {
        printf("명령어가 정상적으로 실행되지 않았습니다!");
        return NULL;
    }

	//이제 결과 값을 읽습니다.
    readSize = fread( (void*)popen_string, sizeof(char), 10240-1, fp );

    pclose(fp);
    popen_string[readSize]=0;

    return popen_string;
}

/*
 * proc 폴더에서 pid들을 가져와 배열에 저장합니다.
 * 이렇게 모은 pid들로 프로세스 관련 데이터를 구합니다.
 */
void get_proc_pid()
{
	FILE * fp = NULL; //popen의 결과값을 얻기 위한 FILE 포인터입니다.
	char * pid_many_string = NULL;
	char * pid_string = NULL;
	char * seperated_pid = NULL; //proc에서 읽은 모든 pid를 파싱해서 나온 한개의 pid 문자열입니다. 이를 int형으로 바꿔서 pid_array에 저장할 것입니다.
	char * delim = "\n"; //전체 pid 문자열을 파싱할 구분자입니다.
	
	int pid_index; //pid를 탐색할 때 사용할 인덱스입니다.
	
	//먼저 pid의 갯수를 구합니다. 
	fp = popen("ls /proc | grep -E ^[0-9] | wc -l","r");
	pid_many_string = popen_result(fp);
	pid_many = atoi(pid_many_string) - 1; //기본적으로  위의 명령어의 결과 뒤에 가독성을 위한 \n이 붙습니다. 이를 감안해서 -1을 합니다.
	pid_many_string = NULL;
	
	fp = popen("ls /proc | grep -E ^[0-9]","r");
	pid_string = popen_result(fp);
	
	pid_array = (int*)malloc(pid_many*sizeof(int)); //이제 pid갯수만큼 배열의 길이를 잡습니다.
	
	seperated_pid = strtok(pid_string,delim);
	if (seperated_pid != NULL && strcmp(seperated_pid, "") != 0)
	{
		pid_array[0] = atoi(seperated_pid);
	}
	
	for (pid_index = 1; pid_index < pid_many; pid_index++)
	{
		seperated_pid = strtok(NULL,delim);
		if (seperated_pid != NULL && strcmp(seperated_pid, "") != 0) //파싱 후에 값이 빈 문자열이거나 NULL이면 오류입니다. 이 부분은 에러를 표시하고 넘어갑니다.
		{
			pid_array[pid_index] = atoi(seperated_pid);
		}
		else
		{
			proc_file_var.proc_error = 1;
		}
	}
	
	pid_string = NULL;
}

/*
 * 해당 프로세스의 status파일을 통해 메모리 크기와 사용자 아이디, 그룹 아이디를 얻습니다.
 */
void get_status_file(int pid_index)
{
	FILE *fp;
	int myint = 1000;
	int BUFFERSIZE=80;
	char proc_path[1024];
	char *buf=malloc(85);
	
	sprintf(proc_path,"/proc/%d/status",pid_array[pid_index]);
	
	if((fp = fopen(proc_path,"r")))
	{
        while(fgets(buf, BUFFERSIZE, fp) != NULL)
        {
                if(strstr(buf, "VmSize") != NULL)
                {
                        if (sscanf(buf, "%*s %d", &myint) == 1)
                        {
								proc_file_var.vmsize = myint;
                        }
                }
                else if(strstr(buf, "UID") !=NULL)
                {
                        if (sscanf(buf, "%*s %d", &myint) == 1)
                        {
								proc_file_var.proc_owner = myint;
                        }
				}
                else if(strstr(buf, "GID") !=NULL)
                {
                        if (sscanf(buf, "%*s %d", &myint) == 1)
                        {
								proc_file_var.proc_group = myint;
                        }
				}
		}
	}
}

void get_time(int pid_index)
{
	char proc_path[1024];
	struct stat buf;
	
	sprintf(proc_path,"/proc/%d",pid_array[pid_index]);
	stat(proc_path,&buf);
	
	proc_file_var.access_proc_time = buf.st_atime;
	proc_file_var.create_proc_time = buf.st_ctime;
	proc_file_var.modifi_proc_time = buf.st_mtime;
}

static struct fuse_operations pfs_oper =
{
	.getattr  = pfs_getattr,
	.readdir  = pfs_readdir,
	.unlink   = pfs_unlink
};

int main (int    argc,
				char **argv)
{
	
	int pid_index;
	
	get_proc_pid();
	
	for (pid_index = 0; pid_index < pid_many; pid_index++)
	{
		proc_file_var.proc_error = 0; //에러를 판단할 때 사용하는 값을 초기화합니다.
		
		get_pid_proc_file_path(pid_index);
		
		if (proc_file_var.proc_error > 0)
		{
			pid_index++;
			continue;
		}
		
		if (proc_file_var.proc_file_path != NULL && proc_file_var.proc_file_path[0] == '/')
		{
			printf("%d번 pid=%d %s\n",pid_index, pid_array[pid_index], proc_file_var.proc_file_path);
		}
		
		//이제 해당 프로세스의 vmsize를 파악합니다.
		get_status_file(pid_index);
		printf("vmsize = %d kb\n",proc_file_var.vmsize);
		printf("사용자 아이디 = %d\n 그룹 아이디 = %d\n", proc_file_var.proc_owner, proc_file_var.proc_group);
		
		//마지막으로 해당 프로세스의 생성 시간, 접근 시간을 파악합니다.
		get_time(pid_index);
		printf("생성 시간 = %d\n 접근 시간 = %d\n\n",proc_file_var.create_proc_time, proc_file_var.access_proc_time);
		
		
	}
	
	return fuse_main (argc, argv, &pfs_oper);
}
