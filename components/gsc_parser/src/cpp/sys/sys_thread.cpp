#include "sys_thread.h"
#include "sys_worker.h"

#if WIN32

int pthread_create(pthread_t* handle, int flags, thread_func_t entryPoint, void* args)
{
	if(*handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)entryPoint, args, 0, NULL))
	{
		return 0;
	}
	
	return GetLastError();
}

int pthread_detach(pthread_t handle)
{
	if(CloseHandle(handle) == FALSE)
	{
		return GetLastError();
	}
	return 0;
}

int pthread_join(pthread_t handle, void** val)
{
	if(WaitForSingleObject(handle, INFINITE) == WAIT_FAILED)
	{
		return GetLastError();
	}
	return CloseHandle(handle);
}

#endif

Thread::Thread(thread_func_t entryPoint, Worker* owner) : owner(owner), argv(NULL)
{
	this->state = THREAD_ALIVE;
	this->state = (pthread_create(&this->handle, 0, entryPoint, this) == 0) * THREAD_ALIVE;
}

Thread::Thread(thread_func_t entryPoint, Worker* owner, void* arg) : owner(owner), argv(arg)
{
	this->state = THREAD_ALIVE;
	this->state = (pthread_create(&this->handle, 0, entryPoint, this) == 0) * THREAD_ALIVE;
}

Thread::~Thread(void)
{
	this->Join();
}

Worker* Thread::Owner(void) const
{
	return this->owner;
} 

bool Thread::IsAlive(void) const
{
	return (this->state & THREAD_ALIVE) != 0;
}

bool Thread::IsExecuting(void) const
{
	return (this->state & THREAD_COMPLETED) != 0;
}

bool Thread::IsDetached(void) const
{
	return (this->owner == NULL) || this->state & THREAD_DETACHED;
}

int Thread::Detach(void)
{
	if(state & (THREAD_DETACHED | THREAD_JOINED))
	{
		return -1;
	}
	
	if(this->owner)
	{
		owner->thread = NULL;
		this->owner = NULL;
	}
	
	int err = pthread_detach(handle);
	if(!err)
	{
		state |= THREAD_DETACHED;
	}
	
	return err;
}

int Thread::Join(void)
{
	void* tmp = NULL;
	return this->Join(&tmp);
}

int Thread::Join(void** value_ptr)
{
	if(this->state & THREAD_JOINED || this->IsDetached())
	{
		return -1;
	}

	this->state |= THREAD_JOINING;
	int err = pthread_join(handle, value_ptr);
	
	if(!err)
	{
		state |= THREAD_JOINED;
	}
	
	return err;
}

void Thread::Kill(void)
{
	this->state &= ~THREAD_ALIVE;
}