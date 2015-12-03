/*
 * OS Assignment #5
 *
 * @file person.c
 */

#include "person.h"
#include <stdio.h>
#include <sys/mman.h>
#include <signal.h>
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
void print_get_attr(const char *attr_name)
{
	int attr_offset = 0; //속성의 오프셋입니다.

	attr_offset = person_get_offset_of_attr (attr_name); //먼저 attr_name을 이용해서 person구조체멤버의 오프셋을 가져옵니다.
	
	//정상적으로 오프셋이 탐색 되었는지 확인합니다.
	if (attr_offset < 0)
	{
		printf("해당 이름의 속성는 없습니다. 출력을 할 수 없습니다.\n");
		return;
	}
	
	/*
	 * 최종 결과물에서는 이 함수를 이용해서 출력해야합니다. 하지만 일단은 디버깅 및 개발의 용이를 위해 각 속성 이름에 따라 출력하도록 만들었습니다.
	if (person_attr_is_integer(attr_name)) //이제 출력할 값이 int인지 문자열인지 파악합니다.
	{
		printf("%d\n", (int *)map_pointer + attr_offset);
	}
	else
	{
		printf("%s\n", (char *)map_pointer + attr_offset);
	}
	*/
	if (!strcmp (attr_name, "name"))
	{
		printf("name: %s\n", (char *)map_pointer + attr_offset);
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
		printf("phone: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
	if (!strcmp (attr_name, "homepage"))
	{
		printf("homepage: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
	if (!strcmp (attr_name, "twitter"))
	{
		printf("twitter: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
	if (!strcmp (attr_name, "facebook"))
	{
		printf("facebook: %s\n", (char *)map_pointer + attr_offset);
		return;
	}
}

/*
 * 성공적으로 값이 변경되었을 땐 바로 이 기쁜 소식을 watchers에 알려야합니다.
 * 이를 위해 watchers들에게 시그널을 보내는 함수가 필요합니다.
 */
void sending_signal(int attr_offset)
{
	pid_t* watchers_pointer = NULL; //watchers배열의 시작 주소입니다.
	sigval_t sigval_val; //sigqueue로 전송될 데이터가 저장될 구조체입니다.
	int watchers_array_index = 0; //watchers 배열을 탐색할 때 사용할 인덱스 변수입니다.
	
	watchers_pointer = map_pointer; //watchers_pointer는 Person 구조체의 첫번째 속성입니다. 따라서 시작주소가 곧 watchers 배열의 시작주소입니다.
	sigval_val.sival_int = attr_offset;
	
	printf("%d\n",watchers_pointer);
	
	while (watchers_array_index < NOTIFY_MAX) //배열을 전부 돌면서 모든 pid에 시그널을 보냅니다.
	{
		watchers_pointer = map_pointer;
		if (watchers_pointer[watchers_array_index] != 0) //해당 부분은 pid가 존재합니다. 해당 pid로 시그널을 보냅니다.
		{
			//sigqueue로 데이터와 시그널을 보냅니다. 만약 시그널 전송에 실패하면 해당 인덱스 부분을 0으로 바꿉니다.
			if (sigqueue( watchers_pointer[watchers_array_index], SIGUSR1,sigval_val) == -1)
			{
				watchers_pointer[watchers_array_index] = 0;
			}
		}
		
		watchers_array_index++;
	}
}

/*
 * 지정한 속성의 값을 변경합니다. 이 함수는 동기화를 위한 상호 배제 기법으로 세마포어를 이용합니다.
 * const char *attr_name: 변경할 속성의 이름입니다.
 * const char *set_value: 변경할 값입니다.
 */
void set_value_attr(const char *attr_name, const char* set_value)
{
	int set_int_value; //int형 데이터는 여기서 저장합니다.
	int attr_offset; //속성의 오프셋입니다.
	//먼저 해당 속성의 offset을 가져옵니다.

	attr_offset = person_get_offset_of_attr(attr_name);
	
	//정상적으로 오프셋이 탐색 되었는지 확인합니다.
	if (attr_offset < 0)
	{
		printf("해당 이름의 속성는 없습니다. 값 변경을 할 수 없습니다.\n");
		return;
	}

	//해당 속성의 타입이 int인지 아닌지 파악합니다.
	if (person_attr_is_integer(attr_name))
	{
		set_int_value = atoi(set_value); //int형 값은 atoi로 변경합니다.
		*((int *)map_pointer + attr_offset) = set_int_value;
	}
	else
	{
		strcpy((char *)map_pointer + attr_offset,set_value);
	}
	
	sending_signal(attr_offset);
}

/*
 * watchers배열에서 빈 인덱스를 찾아 그 자리에 해당 프로세스의 pid를 입력합니다. 만약 빈 자리가 없다면 0번에다 덮어씌웁니다. 
 */
int find_empty_index()
{
	pid_t* watchers_pointer = NULL; //watchers배열의 시작 주소입니다.
	int find_index = 0; //watchers배열의 인덱스입니다.
	
	watchers_pointer = map_pointer; //watchers_pointer는 Person 구조체의 첫번째 속성입니다. 따라서 시작주소가 곧 watchers 배열의 시작주소입니다. 
	
	while (find_index < NOTIFY_MAX)
	{
		if (watchers_pointer[find_index] == 0)
		{
			watchers_pointer[find_index] = getpid();
			printf("watchers [%d] = %d\n",find_index, watchers_pointer[find_index]);
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
	
	if (*(map_pointer + watchers_index * sizeof(pid_t)) != 0)
	{
		*(map_pointer + watchers_index * sizeof(pid_t)) = 0;
	}
	exit(1);
}

/*
 * 시그널을 통해서 받은 offset으로 속성와 
 */
void print_get_offset(int attr_offset, int signal_pid)
{
	char* attr_name = NULL; //속성의 이름입니다.

	attr_name = person_lookup_attr_with_offset(attr_offset); //먼저 attr_name을 이용해서 person구조체멤버의 오프셋을 가져옵니다.
	
	//정상적으로 오프셋이 탐색 되었는지 확인합니다.
	if (attr_name == NULL)
	{
		printf("해당 오프셋의 속성는 없습니다. 출력을 할 수 없습니다.\n");
		return;
	}
	
	if (!strcmp (attr_name, "name"))
	{
		printf("name: %s from %d\n", (char *)map_pointer + attr_offset,signal_pid);
		return;
	}
	if (!strcmp (attr_name, "age"))
	{
		printf("age: %d from %d\n", *(map_pointer + attr_offset),signal_pid);
		return;
	}
	if (!strcmp (attr_name, "gender"))
	{
		printf("gender: %d from %d\n", *(map_pointer + attr_offset),signal_pid);
	}
	if (!strcmp (attr_name, "phone"))
	{
		printf("phone: %s from %d\n", (char *)map_pointer + attr_offset,signal_pid);
		return;
	}
	if (!strcmp (attr_name, "homepage"))
	{
		printf("homepage: %s from %d\n", (char *)map_pointer + attr_offset,signal_pid);
		return;
	}
	if (!strcmp (attr_name, "twitter"))
	{
		printf("twitter: %s from %d\n", (char *)map_pointer + attr_offset,signal_pid);
		return;
	}
	if (!strcmp (attr_name, "facebook"))
	{
		printf("facebook: %s from %d\n", (char *)map_pointer + attr_offset,signal_pid);
		return;
	}
}

/*
 * SIGINT,SIGTERM 시그널의 핸들러 함수입니다. 이 함수는 delete_watchers_index를 호출합니다.
 */
void sigint_sigterm_handler(int signo)
{
	delete_watchers_index();
}

void sigusr1_handler(int signo, siginfo_t *siginfo)
{
	printf("시그널 1 핸들러 작동\n");
	if(siginfo->si_code == SI_QUEUE)
    {
        printf("User RTS signal %d\n", siginfo->si_pid);
        printf("Sig  Number %d\n",     siginfo->si_signo);
        printf("User Data is %d\n",    siginfo->si_value.sival_int);
        // 시그널이 큐잉되는지 확인하기 위한 코드
        
        print_get_offset(siginfo->si_value.sival_int,siginfo->si_pid);
    }
    else
    {
        // kill등을 이용해서 표준 유닉스 시그널을 보냈을 경우
        // 실행되는 루틴 
        printf("Get none realtime signal %d\n", signo); 
    }
}

/*
 * 대기모드에 들어간 프로세스가 등록해야할 시그널 핸들러들을 등록하는 함수입니다.
 */
void signal_regist_watchers()
{
	struct sigaction sigint_struct; //SIGINT 시그널을 처리할 핸들러를 등록하는데 이용할 구조체입니다.
	struct sigaction sigterm_struct; //SIGTERM 시그널을 처리할 핸들러를 등록하는데 이용할 구조체입니다.
	struct sigaction sigusr1_struct; //SIGUSR1 시그널을 처리할 핸들러를 등록하는데 이용할 구조체입니다.
	struct sigaction old_sig_handle; //이전 동작을 지정하는 핸들러인데, 디버깅용이지 특별한 의미를 가지지 않습니다.
	
	sigemptyset(& (sigint_struct.sa_mask));
	sigaddset(& (sigint_struct.sa_mask),SIGINT);
	sigint_struct.sa_handler = sigint_sigterm_handler;
	sigaction(SIGINT,&sigint_struct,&old_sig_handle);
	
	sigemptyset(& (sigterm_struct.sa_mask));
	sigterm_struct.sa_handler = sigint_sigterm_handler;
	sigaction(SIGINT,&sigterm_struct,&old_sig_handle);
	
	sigemptyset(& (sigusr1_struct.sa_mask));
	sigusr1_struct.sa_sigaction = sigusr1_handler;
	sigusr1_struct.sa_flags = SA_SIGINFO;
	
	sigaction(SIGUSR1, &sigusr1_struct, &old_sig_handle);
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
	//int * mmap_pointer; //메모리 매핑을 한 포인터입니다.
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
		signal_regist_watchers(); //그 후, 시그널 핸들러들을 등록합니다.
		
		//이제 무한 루프를 돌게 만듭니다.
		while(1);
		
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
	
	else //set_value가 존재하면 지정한 속성 값을 해당 값으로 변경해야합니다. 이 작업을 시도합니다.
	{
		if (attr_name == NULL)
		{
			print_usage(argv[0]);
			return -1;
		}
		set_value_attr(attr_name, set_value);
		print_get_attr(attr_name);
	}
	


	return 0;
}
