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

int make_proc_id; //생성된 프로세스의 아이디입니다.
char* make_proc_name; //생성된 프로세스의 이름입니다.
char* make_proc_file_name; //위의 두 변수를 합쳐서 만든 파일 이름입니다.
int make_proc_owner; //생성된 프로세스의 사용자 = 파일 소유자입니다.
time_t make_proc_time; //생성된 프로세스의 생성 시간입니다.
time_t access_proc_time; //생성된 프로세스의 최근 접근 시간입니다.

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
  return -ENOENT;
}

static int pfs_unlink (const char *path)
{
  /* not yet implemented */
  return -ENOENT;
}

/*
proc를 제대로 이용하려면 pid를 획득해야합니다. 그 함수가 이 함수입니다.
다음과 같은 순서와 커맨드를 이용해서 작동합니다.
1.ps -o pid= 을 이용해서 현재 실행중인 프로세스의 pid를 전부 구합니다. 이 값은 \n으로 구분됩니다.
2.각 pid들을 구해서 int값으로 만들어(atoi) 배열에 저장합니다.
*/
int get_proc_pid()
{
  //ps커맨드를 실행하는 것 뿐만아니라 그 결과 값을 얻어야함으로 popen함수를 이용합니다.
  char* pid_string = popen("ps -o pid=");
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
