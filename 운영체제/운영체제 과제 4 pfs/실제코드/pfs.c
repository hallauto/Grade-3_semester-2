/*
 * OS Assignment #4
 *
 * @file pfs.c
 */

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int proc_pid; //검색된 프로세스의 아이디입니다.
char* proc_name; //검색된 프로세스의 이름입니다.
char* proc_file_name; //위의 두 변수를 합쳐서 만든 파일 이름입니다.
int proc_owner; //검색된 프로세스의 사용자 = 파일 소유자입니다.
time_t maked_proc_time; //검색된 프로세스의 검색 시간입니다.
time_t access_proc_time; //검색된 프로세스의 최근 접근 시간입니다.
time_t modifi_proc_time; //검색된 프로세스의 수정된 시간입니다.

int pid_many; //전체 pid갯수입니다.
int* pid_array; //pid를 저장할 배열입니다. 모든 pid들이 여기에 저장됩니다.

static int pfs_getattr (const char  *path,
                        struct stat *stbuf)
{
	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0644;
		stbuf->st_nlink = 2;
		stbuf->st_
	} 
	else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
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
  /* not yet implemented */
  /*
  여기에서 각 pid를 이용해서 정보를 얻고 이를 filler에 저장해야합니다.
  이를 위해서 각각의 함수를 만들어야합니다.
  int pid_accesstime() :접속 시간을 반환합니다.
  int pid_modifitime() :수정 시간을 반환합니다.
  int pid_createtime() :생성 시간을 반환합니다.
  char* pid_cmdline() :실행시의 커맨드 라인을 읽습니다.
  char* pid_proc_name() :프로세스의 이름을 읽습니다.
  */
  return -ENOENT;
}

static char* pid_proc_name()
{

}

/*
  pid와 프로세스 이름을 합쳐서 파일 이름을 만듭니다.
*/
static char* make_pid_proc_file_name()
{

}

static int pid_accesstime(const char)
{

}

static int pfs_unlink (const char *path)
{
	/* not yet implemented */
	return -ENOENT;
}


char* popen_result(FILE * fp)
{
	size_t readSize;
	char * popen_string = calloc(10240,sizeof(char)); //결과 문자열을 저장할 변수입니다.

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

static struct fuse_operations pfs_oper =
{
	.getattr  = pfs_getattr,
	.readdir  = pfs_readdir,
	.unlink   = pfs_unlink
};

int main (int    argc,
				char **argv)
{
	return fuse_main (argc, argv, &pfs_oper);
}
