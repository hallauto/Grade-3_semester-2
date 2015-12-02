/*
 * OS Assignment #5
 *
 * @file person.c
 */

#include "person.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int* map_pointer = NULL; //매핑된 주소 포인터입니다.
int watchers_index; //watchers 배열의 인덱스입니다. 이후에 대기 모드 프로세스가 종료될 때, 이 부분을 0으로 변환합니다.

/*
 * 주어진 매개변수와 지정된 값을 활용해서 메모리 맵을 지정합니다.
 */
void * allocate_mmap(int file_descriptor)
{
	if (file_descriptor < 0)
	{
		fprintf(stderr,"파일 디스크립터에 문제가 있습니다.\n");
		return NULL;
	}
	
	map_pointer = mmap(0, sizeof(Person), PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor,0);
	
	if (map_pointer == MAP_FAILED)
	{
		fprintf(stderr,"메모리 매핑중에 문제가 발생했습니다.\n");
		return NULL;
	}
	
	return map_pointer;
}

/*
  * 해당하는 이름의 파일이 존재하는지 파악합니다.
  * 만약 없다면 새로 생성하고 Person구조체 크기만큼 0x00 = 0으로 채웁니다.
  */
void check_file_exist(const char* file_name)
{
	FILE * file_pointer = NULL; //파일이 존재하지 않을 때, 새로 파일을 생성하고 내용을 초기화할 때 쓸 FILE 포인터입니다.
	int* empty_array; //파일을 초기화할 떄 쓸 배열입니다.
	
	//먼저 해당 파일이 존재하는지 확인합니다. 만약 존재하지 않으면 새로 생성하고 sizeof(Person)만큼 0을 입력합니다.
	if (access(file_name, F_OK) == -1)
	{
		file_pointer = fopen(file_name,"w+");
		empty_array = calloc(sizeof(Person) + 1, sizeof(char));
		fwrite(empty_array,1,sizeof(Person),file_pointer);
		fclose(file_pointer);
	}


	return;
}
 
/*
 * attr_name 값 = 지정된 속성 이름' 의 값을 매핑된 파일에서 가져와 출력합니다.
 * const char *attr_name: 지정된 속성 이름입니다.
 */
static void print_get_attr(const char *attr_name)
{
	int attr_offset = 0; //속성의 오프셋입니다.

	attr_offset = person_get_offset_of_attr (attr_name); //먼저 attr_name을 이용해서 person구조체의 멤버를 파일에서 가져옵니다.
	
	/*
	 * 최종 결과물에서는 이 함수를 이용해서 출력해야합니다. 하지만 일단은 디버깅 및 개발의 용이를 위해 각 속성 이름에 따라 출력하도록 만들었습니다.
	if (person_attr_is_integer(attr_name))
	{
		
	}
	*/
	if (!strcmp (attr_name, "name"))
	{
		printf("name: %s\n", (char *)map_pointer);
		return;
	}
	if (!strcmp (attr_name, "age"))
	{
		printf("age: %d\n", *(map_pointer + attr_offset));
		return;
	}
	if (!strcmp (attr_name, "gender"))
	{
		printf("gender: %d\n", *(map_pointer + attr_offset));
	}
	if (!strcmp (attr_name, "phone"))
	{
		printf("name: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
	if (!strcmp (attr_name, "homepage"))
	{
		printf("name: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
	if (!strcmp (attr_name, "twitter"))
	{
		printf("name: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
	if (!strcmp (attr_name, "facebook"))
	{
		printf("name: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
}

//주석 작성 합시다.
static void set_value_attr(const char *attr_name, const char* set_value)
{
	int set_int_value; //int형 데이터는 여기서 저장합니다.
	int attr_pointer; //속성의 포인터입니다.
	//먼저 해당 멤버변수의 offset을 가져옵니다.

	attr_pointer = person_get_offset_of_attr(attr_name);

	//해당 멤버변수의 타입이 int인지 아닌지 파악합니다.
	if (person_attr_is_integer(attr_name))
	{
		set_int_value = atoi(set_value);
	}
	else
	{
		
	}
}

//주석 작성 합시다.
int find_empty_index()
{
	pid_t* watchers_pointer = NULL; //watchers배열의 시작 주소입니다.
	int find_index = 0; //watchers배열의 인덱스입니다.
	
	watchers_pointer = map_pointer; //watchers_pointer는 Person 구조체의 첫번째 멤버변수입니다. 따라서 시작주소가 곧 watchers 배열의 시작주소입니다. 
	
	while (find_index < NOTIFY_MAX)
	{
		if (*(watchers_pointer + find_index * sizeof(pid_t)) == 0)
		{
			*(watchers_pointer + find_index * sizeof(pid_t)) = getpid();
			printf("watchers [%d] = %d\n",find_index, *(watchers_pointer + find_index * sizeof(pid_t)));
			return find_index;
		}
		
		find_index++;
	}
	
	//반복문을 벗어났다는 것은 현재 비어있는 공간이 없다는 뜻입니다. 따라서 요구사항대로 0번에 저장합니다.
	find_index = 0;
	*watchers_pointer = getpid();
	
	printf("watchers [%d] = %d\n",find_index, *(watchers_pointer + find_index * sizeof(pid_t)));
	return 0;
}

/*
 * 대기모드 프로세스가 종료될 때, 해당 watchers 배열 인덱스의 값을 0으로 되돌립니다.
 */
void delete_watchers_index()
{
	
	if (*(map_pointer + watchers_index * sizeof(pid_t)) == 0)
	{
		return;
	}
	else
	{
		*(map_pointer + watchers_index * sizeof(pid_t)) = 0;
	}
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
	int * mmap_pointer; //메모리 매핑을 한 포인터입니다.
	const char *file_name;
	const char *set_value;
	const char *attr_name;
	int         watch_mode;
	
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
    
	if (!watch_mode && (optind >= argc))
	{
		print_usage (argv[0]);
		return -1;
	}
	attr_name = argv[optind];

	/* not yet implemented */
	check_file_exist(file_name); //먼저 파일이 존재하는지 검사하고 없으면 생성하는 check_file_exist함수를 호출합니다.
	file_pointer = fopen(file_name,"r+");
	if (file_pointer == NULL)
	{
		print_usage(argv[0]);
		return -1;
	}
	
	mmap_pointer = NULL;
	
	//mmap을 이용해서 메모리 매핑을 합니다.  만약 반환값이 NULL이면 에러이므로 종료합니다.
	if (allocate_mmap(fileno(file_pointer)) == NULL)
	{
		printf("메모리 매핑중에 문제가 발생했습니다.\n");
		return -1;
	}	
	
	//watch_mode가 1이면 이 프로세스의 pid를 watchers 배열에 저장해야합니다. 또, 시그너 핸들러를 등록해야합니다. 이후에 이 프로그램은 무한 루프를 돕니다.
	if (watch_mode == 1)
	{
		watchers_index = find_empty_index(); //먼저 pid를 watchers에 저장하기 위해 빈 공간의 인덱스를 찾습니다.
		
		
		
		delete_watchers_index();
		return 0;
	}
	
	if (set_value == NULL) //set_value가 존재하는지 검사합니다. 만약 존재하지 않으면 지정한 속성 값을 출력합니다.
	{
		if (attr_name == NULL)
		{
			print_usage(argv[0]);
			return -1;
		}
		print_get_attr(attr_name);
	}
	


	return 0;
}
