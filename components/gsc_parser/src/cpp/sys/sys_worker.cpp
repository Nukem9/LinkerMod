#include "sys_worker.h"
#include <errno.h>

int Worker::count = 0;

sem_t Job::sem_access;
sem_t Job::sem_jobs;

Job Job::queue(NULL, NULL);

Job::Job() : func(NULL) , argv(NULL)
{
	this->SetOwner(this);
}

Job::Job(job_func_t job_func, void* job_args) : func(job_func), argv(job_args)
{
	this->SetOwner(this);
}

Job::~Job()
{
}

void Job::Register()
{
	LockAccess();
	
	queue.AddToEnd(this);
	sem_post(&sem_jobs);
	
	UnlockAccess();
}


void Job::PostQuitJob()
{
	Job::LockAccess();
	
	queue.flags |= (JOB_QUIT | JOB_STATIC);
	for(Job* j = queue.NextElem(); j; j = queue.NextElem())
	{
		delete j;
	}
	
	// Tell the workers that there is a job waiting
	// and then kill them when they find it
	sem_post(&sem_jobs);
	
	Job::UnlockAccess();
}

void Job::LockAccess(void)
{
	sem_wait(&sem_access);
}

void Job::UnlockAccess(void)
{
	sem_post(&sem_access);
}

Worker::Worker()
{
	if(!(this->id = Worker::count++))
	{
		sem_init(&Job::sem_jobs, 0, 0);
		sem_init(&Job::sem_access, 0, 1);
	}
	
	//printf("Worker[%d]: Init\n", this->id);
	this->thread = new Thread(Worker::JobHandler, this);
}

Worker::~Worker()
{
	//printf("Worker[%d]: Free\n", id);

	if(this->thread && thread->state == THREAD_ALIVE)
	{
		//thread->Kill();
		thread->Join();
		//printf("Worker[%d]: Free %d\n", id, this->thread->state);

	}
	delete this->thread;
	
	if(!--Worker::count)
	{
		sem_destroy(&Job::sem_access);
		sem_destroy(&Job::sem_jobs);
	}
}

void* THREAD_CALL Worker::JobHandler(void* args)
{
	Thread* thread = (Thread*)args;
	
	//printf("Worker[%d]: Alive\n", id);
	while(thread->IsAlive())
	{
		sem_wait(&Job::sem_jobs);
		//printf("Worker[%d]: Job available\n", thread->owner->id);
		
		Job::LockAccess();
		//printf("Worker[%d]: Aquired access %d\n", thread->owner->id, thread->IsAlive());
		
		//Get the job stuff - pop it from the list
		
		// Check for quit message
		if(Job::queue.flags & JOB_QUIT)
		{
			thread->Kill();
			sem_post(&Job::sem_jobs);
			Job::UnlockAccess();
			continue;
		}
		
		Job* job = Job::queue.NextElem();
		if(job)
		{
			job->Remove();
		}
		else
		{
			//printf("Worker[%d]: Job error - invalid job\n", thread->owner->id);
		}
		
		Job::UnlockAccess();
		
		job->func(job->argv);
		delete job;
		
		if(!thread->IsAlive())
		{
			continue;
		}
	}
	
	if(~thread->state & THREAD_JOINING)
	{
		thread->Detach();
		delete thread;
		//printf("Worker[%d]: Exit via Detach\n", thread->owner->id);
	}
	
	//printf("Worker[%d]: Exit via JOIN\n", thread->owner->id);
	
	return 0;
}

void Worker::Detach()
{
	if(!this->thread)
	{
		return;
	}
	
	thread->Detach();
	this->thread = NULL;
}

void Worker::Kill()
{
	if(this->thread)
	{
		this->thread->Kill();
	}
}

void* Worker::Join()
{
	if(this->thread)
	{
		void* out = NULL;
		thread->Join(&out);
		return out;
	}
	
	return NULL;
}

int Worker::Count(void)
{
	return Worker::count;
}
