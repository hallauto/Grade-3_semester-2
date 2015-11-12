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

int make_proc_id;
char* make_proc_name;
time_t make_proc_time;
time_t access_proc_time;

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
