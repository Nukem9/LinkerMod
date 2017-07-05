#include "sys_semaphore.h"

#if WIN32

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
	if(*sem = CreateSemaphore(NULL, value, 999, NULL))
	{
		return 0;
	}
	
	return -1;
}

int sem_destroy(sem_t *sem)
{
	if(CloseHandle(*sem))
	{
		return 0;
	}
	
	return GetLastError();
}

int sem_wait(sem_t *sem)
{
	if(WaitForSingleObject(*sem, INFINITE) == WAIT_FAILED)
	{
		return GetLastError();
	}
	
	return 0;
}

int sem_post(sem_t *sem)
{
	long val = 0;
	if(!ReleaseSemaphore(*sem, 1, &val))
	{
		return GetLastError();
	}
	
	return 0;
}

#endif
