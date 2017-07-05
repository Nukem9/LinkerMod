#pragma once

#include <stdio.h>
#include "sys_thread.h"
#include "sys_semaphore.h"

#include "../util/llist.h"

typedef int (*job_func_t)(void* argv);

enum JOB_FLAGS
{
	JOB_QUIT			=	1 << 0,
	JOB_STATIC			=	1 << 1, // Do not remove the job from the queue
	//JOB_SYNCRONOUS	=	1 << 2, // Execute the job on the main thread
};

class Job : public LList<Job>
{
friend class Worker;
	
private:
	static sem_t sem_access;
	static sem_t sem_jobs;

	static Job	queue;

	job_func_t	func;
	void*		argv;
	
	int			flags;
	
public:
	Job();
	Job(job_func_t job_func, void* job_args);
	~Job();
	
	void Register(void);
	void Unregister(void);
	
	static void PostQuitJob();

	static void AddJob();
	
	//
	// Blocks the calling thread until it aquires access to the job list 
	//
	static void LockAccess(void);
	static void UnlockAccess(void);
};

class Worker
{
friend class Job;
friend class Thread;
private:
	static int count;
	static void* THREAD_CALL JobHandler(void* args);
	
	int id;
	
	Thread* thread;
	
public:
	Worker();
	~Worker();
	
	//
	// Close the worker thread after current job
	//
	void Detach();
	
	//
	// Block the calling function until the worker thread completes and is closed
	//
	void* Join();
	
	//
	// Kill the worker thread
	//
	void Kill();
	
	//
	// Returns the current number of workers
	//
	static int Count(void);
};
