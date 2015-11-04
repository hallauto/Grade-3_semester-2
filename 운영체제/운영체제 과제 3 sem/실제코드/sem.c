/*
 * OS Assignment #3
 *
 * @file sem.c
 */

#include "sem.h"
#include <stdlib.h>

struct test_semaphore
{
 	pthread_mutex_t mutext_value; //insert_value의 연산을 원자화 하기위한 mutext 구조체입니다.
 	int insert_value; //입력된 세마포어 값입니다. 이 값을 기준으로 프로그램이 동작합니다.
};

tsem_t * tsem_new (int value)
{
  /* not yet implemented. */

  return NULL;
}

void tsem_free (tsem_t *sem)
{
  /* not yet implemented. */
}

void tsem_wait (tsem_t *sem)
{
  /* not yet implemented. */
}

int tsem_try_wait (tsem_t *sem)
{
  /* not yet implemented. */

  return 1;
}

void tsem_signal (tsem_t *sem)
{
  /* not yet implemented. */
}
