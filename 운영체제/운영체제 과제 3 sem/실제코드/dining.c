/*
 * OS Assignment #3
 *
 * @file dininig.c
 */

#include "sem.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static tsem_t *chopstick[5]; //철학자들이 쓸 젓가락입니다. 이들은 각 철학자에게 2개씩 주어져야하고, 철학자들은 무조건 자기 앞자리의 젓가락만 잡습니다.
static tsem_t *updating; //상황을 업데이트할 때 상호배제를 하기위한 구조체입니다.

//현재 상황을 파악하고 이에 맞춰 값을 입력합니다.
static int update_status (int i, int eating)
{
  static int status[5] = { 0, }; //각 철학자들의 상황을 저장한 값입니다. 밥먹으면 1입니다.
  static int duplicated; //옆 사람도 밥을 먹고있는지 검사하는 함수입니다. 당연히 이건 불가능 합니다.
  int idx;
  int sum; //사용중인 젓가락의 합입니다.

  tsem_wait (updating); //업데이트를 위한 정지입니다. 이후 56번째 줄까지 임계 구역(critical_section)입니다.

  status[i] = eating; //주어진 값을 입력합니다.

  /* Check invalid state. */
  duplicated = 0;
  sum = 0;
  for (idx = 0; idx < 5; idx++)
    {
      sum += status[idx];
      if (status[idx] && status[(idx + 1) % 5]) //옆 사람이 밥을 먹고있는지 검사합니다. 만약 그렇다면 상호배제에 문제가 있었다는 뜻입니다.
	duplicated++;
    }

  /* Avoid printing empty table. */
  if (sum == 0) //아무도 밥을 먹고 있지 않군요. 이 때는 출력하는 의미가 없습니다. 출력을 하지말고 바로 return합니다.
    {
      tsem_signal (updating); //lock을 풀고 리턴합니다.
      return 0;
    }

  for (idx = 0; idx < 5; idx++)
    fprintf (stdout, "%3s     ", status[idx] ? "EAT" : "..."); //status가 1이라면 밥을 먹는 중입니다.

  /* Stop on invalid state. */
  if (sum > 2 || duplicated > 0) //젓가락이 2개 이상이거나, 옆 사람이 밥을 먹고 있는 중이라면 오류입니다.
    {
      fprintf (stdout, "invalid %d (duplicated:%d)!\n", sum, duplicated);
      exit (1);
    }
  else
    fprintf (stdout, "\n");

  tsem_signal (updating);

  return 0;
}

//스레드는 다음 함수를 작동시킵니다. 이제 여기에서 중요한 요소를 찾아봅시다.
void * thread_func (void *arg)
{
  int i = (int) (long) arg; //스레드 생성 순서대로 주어진 인덱스입니다.
  int k = (i + 1) % 5; //k는 그 옆의 인덱스입니다. 즉, 어떤 철학자 스레드가 잡는 젓가락은 생성 순서대로 주어진 인덱스 젓가락과 그 옆(= +1 된) 젓가락입니다.
  
  int blocked_i; //i번째 젓가락이 사용중인지(=lock 된지)를 알려주는 변수입니다. 1이면 lock, 아니면 unlock 상태입니다.
  int blocked_k; //k번째 젓가락이 사용중인지(=lock 된지)를 알려주는 변수입니다. 1이면 lock, 아니면 unlock 상태입니다.

	//이 프로그램은 무한히 작동하면서 철학자들의 신나는 파티를 표현할 것입니다.
  do
    {
			blocked_i = tsem_try_wait(chopstick[i]);
			blocked_k = tsem_try_wait(chopstick[k]);
			if (blocked_i || blocked_k) //철학자들은 두 젓가락을 동시에 얻지 못하면 전부 반납합니다. 따라서, 젓가락 2개를 적절하게 얻은 사람이 적절하게 밥을 먹게됩니다.
			{
				if (!blocked_i) //먼저 i번째 젓가락이 lock이 되었는지 파악합니다. lock이라면 이를 반납합니다.
					tsem_signal(chopstick[i]);
				else if (!blocked_k) //이번엔 k번째 젓가락이 lock인지 파악합니다. lock이라면 이를 반납합니다.
					tsem_signal(chopstick[k]);
				continue;
			}
			update_status (i, 1); //일단 젓가락을 얻는데 성공하면 이후에는 해당 값에 따라 update_status 함수를 부릅니다.
			update_status (i, 0); //이제 밥을 다 먹었습니다. 이 상황을 update_status 함수로 입력합니다.
			tsem_signal (chopstick[i]);
			tsem_signal (chopstick[k]);
    }
  while (1);
  
  return NULL;
}

int main (int    argc, char **argv)
{
  int i;

//먼저 각각의 젓가락을 tsem_new로 상호배제가 되는 자원으로 만듭니다.
  for (i = 0; i < 5; i++)
    chopstick[i] = tsem_new (1);
  updating = tsem_new (1); //또한 업데이트를 할 때 쓸 세마포어도 만듭니다.

  for (i = 0; i < 5; i++)
    {
      pthread_t tid;

      pthread_create (&tid, NULL, thread_func, (void *) (long) i); //스레드를 만들고 실행합니다.
    }

  /* endless thinking and eating... */
  while (1)
    usleep (10000000); //이제 메인 스레드는 무한히 작동합니다.(그래야 다른 스레드들이 계속 밥을 먹겠죠?)
  
  return 0;
}
