/*
 * OS Assignment #1 Test Task.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

static char *name = "Task";
static volatile int looping;

static void signal_handler (int signo)
{
  if (!looping)
    return;

  looping = 0;
  fprintf (stderr, "'%s' terminated by SIGNAL(%d)\n", name, signo);
}

int main (int    argc, char **argv)
{
  int timeout = 0;
  int read_stdin = 0;
  char *msg_stdout = NULL;

  /* Parse command line arguments. */
  {
    int opt; //주어진 옵션을 저장할 변수입니다.

    while ((opt = getopt (argc, argv, "n:t:w:r")) != -1) //옵션이 n,t,w,r이 맞다면 다음을 합니다.
      {
	switch (opt)
	  {
	  case 'n':
	    name = optarg; //옵션에 n이 있으면 optarg에서 주어진 이름이 이 프로그램의 새로운 이름입니다. 이를 저장합니다.
	    break;
	  case 't':
	    timeout = atoi (optarg); //옵션에 t가 있으면 타임아웃 값을 지정합니다.
	    break;
	  case 'r':
	    read_stdin = 1; //옵션에 r이 있으면 표준 입력장치를 그대로 1로 둡니다.
	    break;
	  case 'w':
	    msg_stdout = optarg; //stdout에 출력할 문자열은 optarg에 있습니다.
	    break;
	  default:
	    fprintf (stderr, "usage: %s [-n name] [-t timeout] [-r] [-w msg]\n", argv[0]); //opt에 잘못된 값이 들어왔으면 stderr에 다음과 같이 출력합니다.
	    return -1;
	  }
      }
  }

  /* Register SIGINT / SIGTERM signal handler. */
  {
    struct sigaction sa;
    
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = signal_handler;
    if (sigaction (SIGINT, &sa, NULL))
      {
	fprintf (stderr, "'%s' failed to register signal handler for SIGINT\n", name);
	return -1;
      }

    sigemptyset (&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = signal_handler;
    if (sigaction (SIGTERM, &sa, NULL))
      {
	fprintf (stderr, "'%s' failed to register signal handler for SIGTERM\n", name);
	return -1;
      }
  }

  fprintf (stderr, "'%s' start (timeout %d)\n", name, timeout);

  looping = 1;

  /* Write the message to standard outout. */
  if (msg_stdout) //출력할 메시지가 있으면 이를 출력합니다.
    {
      char msg[256];

      snprintf (msg, sizeof (msg), "%s from %s", msg_stdout, name);
      write (1, msg, strlen (msg) + 1);
    }
  
  /* Read the message from standard input. */
  if (read_stdin) //입력장치가 존재하면 메시지를 입력받습니다.
    {
      char    msg[256];
      ssize_t len;

      len = read (0, msg, sizeof (msg));
      if (len > 0)
	{
	  msg[len] = '\0';
	  fprintf (stderr, "'%s' receive '%s'\n", name, msg);
	}
    }

  /* Loop */
  while (looping && timeout != 0) //타임아웃이 넘어갈 때까지 이 루프를 돕니다.
    {
      if (0) 
       fprintf (stderr, "'%s' timeout %d\n", name, timeout);
      usleep (1000000);
      if (timeout > 0)
	     timeout--;
    }

  fprintf (stderr, "'%s' end\n", name); //이제 Task가 끝났습니다. stderr에 이 사실을 출력합니다.

  return 0;
}
