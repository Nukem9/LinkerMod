#pragma once

#if WIN32
	#include <Windows.h>

	typedef HANDLE sem_t;

	int sem_init(sem_t *sem, int pshared, unsigned int value);
	int sem_destroy(sem_t *sem);
	int sem_wait(sem_t *sem);
	int sem_post(sem_t *sem);
#else
	#include <semaphore.h>
#endif

