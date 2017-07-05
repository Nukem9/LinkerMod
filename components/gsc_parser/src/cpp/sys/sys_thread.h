#pragma once

#if WIN32
	#include <Windows.h>
	typedef HANDLE pthread_t;
	
	#define THREAD_CALL WINAPI
	typedef void* (THREAD_CALL *thread_func_t)(void* args);

	int pthread_create(pthread_t* handle, int flags, thread_func_t entryPoint, void* args);
	int pthread_detach(pthread_t handle);
	int pthread_join(pthread_t handle, void** val);
#else
	#include <pthread.h>

	#define THREAD_CALL 
	typedef void* (*thread_func_t)(void* args);
#endif

class Worker;

enum THREAD_STATE_FLAGS
{
	THREAD_ALIVE		= 1 << 0,
	THREAD_JOINING		= 1 << 1,
	THREAD_JOINED		= 1 << 2,
	THREAD_DETACHED		= 1 << 3,
	THREAD_COMPLETED	= 1 << 4,
};

class Thread
{
friend class Worker;
private:
	pthread_t handle;
	Worker* owner;

	void* argv;

protected:
	volatile int state;
	bool completed;
	
public:
	Thread(thread_func_t entryPoint, Worker* owner);
	Thread(thread_func_t entryPoint, Worker* owner, void* argv);
	virtual ~Thread(void);
	
	Worker* Owner(void) const;
	
	bool IsAlive(void) const;
	bool IsExecuting(void) const;
	bool IsDetached(void) const;
	
	int Detach(void);
	int Join(void);
	int Join(void** value_ptr);
	
	void Kill(void);
};
