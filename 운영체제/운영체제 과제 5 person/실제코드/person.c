/*
 * OS Assignment #5
 *
 * @file person.c
 */

#include "person.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int person_size; //Person 객체의 크기입니다.
int map_pointer; //매핑된 주소 포인터입니다.

/*
 * 주어진 매개변수와 지정된 값을 활용해서 메모리 맵을 지정합니다.
 * 기본적으로 프로그램은 
 */
void* allocate_mmap(int file_descriptor)
{
	if (file_descriptor < 0)
	{
		fprintf(stderr,"파일 디스크립터에 문제가 있습니다.");
		return -1;
	}
	
	map_pointer = mmap(0, person_size + 1, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor,0);
	
	if (map_pointer == MAP_FAILED)
	{
		fprintf(stderr,"메모리 매핑중에 문제가 발생했습니다.");
		return -1;
	}
	
	return map_pointer;
}

/*
  * 해당하는 이름의 파일이 존재하는지 파악합니다.
  * 만약 없다면 새로 생성하고 Person구조체 크기만큼 0x00 = 0으로 채웁니다.
  */
void check_file_exist(const char* file_name)
{
  int empty_index; //새로 생성할때 이용할 인덱스입니다.
  if (access(file_name, F_OK) == -1) //먼저 해당 파일이 존재하는지 확인합니다. 만약 존재하지 않으면 새로 생성하고 person_size만큼 0을 입력합니다.
  {
    file_pointer = fopen(file_name,"w+");
    empty_array = calloc(person_size+1, sizeof(char));
    for (empty_index = 0; empty_index < person_size + 1; empty_index++)
    {
      empty_array[empty_index] = 0;
    }
    fwrite(empty_array,1,person_size,file_pointer);
    fclose(file_pointer);
  }


  return;
}

/*
  * 'attr_name 값 = 지정된 속성 이름' 의 값을 매핑된 파일에서 가져와 출력합니다.
  * const char *attr_name: 지정된 속성 이름입니다.
  */
static void print_get_attr(const char *attr_name)
{
  int attr_pointer = NULL; //속성의 포인터입니다.

  attr_pointer = person_get_offset_of_attr (attr_name); //먼저 attr_name을 이용해서 person구조체의 멤버를 파일에서 가져옵니다.
}

static void
print_usage (const char *prog)
{
	fprintf (stderr, "usage: %s [-f file] [-w] [-s value] attr_name\n", prog);
	fprintf (stderr, "  -f file  : person file, default is './person.dat'\n");
	fprintf (stderr, "  -w       : watch mode\n");
	fprintf (stderr, "  -s value : set the value for the given 'attr_name'\n");
}

int
main (int    argc,
      char **argv)
{
	FILE * file_pointer;
	const char *file_name;
	const char *set_value;
	const char *attr_name;
	int         watch_mode;
	
	char* empty_array;

	person_size = sizeof(Person);
	/* Parse command line arguments. */
	file_name  = "./person.dat";
	set_value  = NULL;
	watch_mode = 0;
	while (1)
	{
		int opt;

		opt = getopt (argc, argv, "ws:f:");
		if (opt < 0)
			break;

		switch (opt)
		{
		case 'w':
			watch_mode = 1;
			break;
		case 's':
			set_value = optarg;
			break;
		case 'f':
			file_name = optarg;
			break;
		default:
			print_usage (argv[0]);
			return -1;
			break;
		}
    }
    
	if (!watch_mode && (optind > argc))
	{
		print_usage (argv[0]);
		return -1;
	}
	attr_name = argv[optind];

	/* not yet implemented */
	file_pointer = fopen(file_name,"w+");
	if (file_pointer == NULL)
	{
		print_usage(argv[0]);
		return -1;
	}
	
	//mmap을 이용해서 메모리 매핑을 합니다.  만약 반환값이 -1이면 에러이므로 종료합니다.
	if (allocate_mmap(fileno(file_pointer)) == -1)
		return -1;
	
	if (set_value == NULL)
	{
    print_get_attr(attr_name);
    
	}
	


	return 0;
}
