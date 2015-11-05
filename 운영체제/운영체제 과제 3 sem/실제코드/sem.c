/*
 * OS Assignment #3
 *
 * @file sem.c
 */

#include "sem.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*
이 구조체는 insert_value를 조작할 때 상호배제가 보장되며, 이는 mutex로 구현되어있습니다.
*/
struct test_semaphore
{
	pthread_cond_t cond_value; //test관련 연산들의 대기열 관리를 위한 조건변수 구조체입니다.
	//pthread_mutex_t cond_mutex_value; //test관련 연산들의 대기열 관리를 위한 mutex_t 구조체입니다.

	pthread_mutex_t mutex_value; //해당 조건 변수의 mutex_t 구조체입니다.
 	int insert_value; //입력된 세마포어 값입니다. 이 값을 조작하는 부분이 원자화 되어야합니다. 또, 이 값이 0이하라면 lock이 걸려야합니다.
};

tsem_t * tsem_new (int value)
{
	tsem_t * new_sem = calloc(1,sizeof(tsem_t)); //동적으로 메모리를 할당합니다.
	
	pthread_cond_init(&(new_sem->cond_value),NULL); //조건 변수를 동적으로 할당합니다.
	//pthread_mutex_init(&(new_sem->cond_mutex_value),NULL);
	pthread_mutex_init(&(new_sem->mutex_value),NULL); //mutex를 동적으로 할당합니다.
	
	new_sem->insert_value = value; //지정된 세마포어 값을 저장합니다.


  return new_sem;
}

void tsem_free (tsem_t *sem)
{
	pthread_mutex_destroy(&(sem->mutex_value)); //먼저 mutex 제거를 시도합니다.
	pthread_cond_destroy(&(sem->cond_value));
	free(sem);
}

void tsem_wait (tsem_t *sem)
{
	/*순서대로 다음과 같이 이루어져야합니다.
	1. 먼저 insert_value가 원자적으로 조작되도록 mutex를 이용합니다.
	2. 성공적으로 mutex의 lock을 얻었다면 이제 insert_value의 값을 검사합니다.
	3-1. 만약 0이하라면 -1을 하고 cond_value를 이용해서 lock에 들어갑니다.
	3-2. 만약 0 초과라면 -1을 하고 그대로 프로그램은 동작합니다.
	*/
	int status; //mutex를 통해 lock을 획득하려고 시도한 결과 값입니다.

	status = pthread_mutex_lock(&(sem->mutex_value));
	
	//critical section(임계 영역)
	if (status != 0)
	{
		fprintf(stderr,"tsem_wait에서 mutex_value의 lock에 error가 있습니다.");
	}
	
	sem->insert_value--;
	if (sem->insert_value < 0)
	{
		//status = pthread_mutex_unlock(&(sem->mutex_value));
		//status = pthread_mutex_lock(&(sem->cond_mutex_value));
		pthread_cond_wait(&(sem->cond_value),&(sem->mutex_value));
		//status = pthread_mutex_unlock(&(sem->cond_mutex_value));
	}
	
	status = pthread_mutex_unlock(&(sem->mutex_value));
	//critical section(임계 영역) 끝
}

int tsem_try_wait (tsem_t *sem)
{
	/*순서대로 다음과 같이 이루어져야합니다.
	1. 먼저 insert_value가 원자적으로 조작되도록 mutex를 이용합니다.
	2. 성공적으로 mutex의 lock을 얻었다면 이제 insert_value의 값을 검사합니다.
	3-1. 만약 0이하라면 -1을 하고 cond_value를 이용해서 lock에 들어가...는게 아니고 1을 반환합니다.
	3-2. 만약 0 초과라면 -1을 하고 0을 반환합니다.
	*/
	int status; //mutex를 통해 lock을 획득하려고 시도한 결과 값입니다.
	
	status = pthread_mutex_lock(&(sem->mutex_value));
	
	//critical section(임계 영역)
	if (status != 0)
	{
		//fprintf(stderr,"tsem_wait에서 mutex_value의 lock에 error가 있습니다.");
	}
	if (sem->insert_value > 0) //세마포어 값이 0보다 크면 1을 감소하고 0을 반환합니다.(wait할 필요가 없습니다.)
	{
		//critical section(임계 영역) 끝
		status = pthread_mutex_unlock(&(sem->mutex_value));
		tsem_wait(sem);
		return 0;
	}
	else //세마포어 값이 0 이하면 1을 반환합니다.(이 자원이 lock되어 있음을 알려주는 것입니다.)
	{
		//critical section(임계 영역) 끝
		status = pthread_mutex_unlock(&(sem->mutex_value));
		return 1;
	}
	
}

void tsem_signal (tsem_t *sem)
{
	/*순서대로 다음과 같이 이루어져야합니다.
	1. 먼저 insert_value가 원자적으로 조작되도록 mutex를 이용합니다.
	2. 성공적으로 mutex의 lock을 회수하였다면 이제 insert_value의 값을 검사합니다.
	3-1. 만약 0 이하라면 +1을 하고 cond_value를
	3-2. 만약 0 초과라면 시그널을 보내서 thread들을 전부 깨웁니다.
	*/
	
	int status; //mutex를 통해 lock을 회수하려고 시도한 결과 값입니다.

	status = pthread_mutex_lock(&(sem->mutex_value));
	
	//critical section(임계 영역)
	if (status != 0)
	{
		fprintf(stderr,"tsem_wait에서 mutex_value의 lock에 error가 있습니다.");
	}
	sem->insert_value++;
	if (sem->insert_value <= 0)
	{
		pthread_cond_signal(&(sem->cond_value));
	}
	
	status = pthread_mutex_unlock(&(sem->mutex_value));
	//critical section(임계 영역) 끝
}
