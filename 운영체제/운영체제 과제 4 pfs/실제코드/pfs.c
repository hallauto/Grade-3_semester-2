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

int* proc_pid_names
int proc_pid; //검색된 프로세스의 아이디입니다.
char* proc_name; //검색된 프로세스의 이름입니다.
char* proc_file_name; //위의 두 변수를 합쳐서 만든 파일 이름입니다.
int proc_owner; //검색된 프로세스의 사용자 = 파일 소유자입니다.
time_t maked_proc_time; //검색된 프로세스의 검색 시간입니다.
time_t access_proc_time; //검색된 프로세스의 최근 접근 시간입니다.
time_t modifi_proc_time; //검색된 프로세스의 수정된 시간입니다.

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

/*
proc를 제대로 이용하려면 pid를 획득해야합니다. 그 함수가 이 함수입니다.
다음과 같은 순서와 커맨드를 이용해서 작동합니다.

2.각 pid들을 구해서 int값으로 만들어(atoi) 배열에 저장합니다.
*/
int get_proc_pid()
{
  pid_many = popen("ls /proc | grep ^[0-9]*$ | wc -l");
  char * pid_string = popen("ls /proc | grep ^[0-9]*$");
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
