/*
    Thread Pool implementation for unix / linux environments
    Copyright (C) 2008 Shobhit Gupta
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "threadpool.h"

using namespace std;

pthread_mutex_t ThreadPool::mutexSync = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::mutexWorkCompletion = PTHREAD_MUTEX_INITIALIZER;



ThreadPool::ThreadPool()
{
    ThreadPool(2);
}

ThreadPool::ThreadPool(int maxThreads)
{
   if (maxThreads < 1)  maxThreads=1;
  
   //mutexSync = PTHREAD_MUTEX_INITIALIZER;
   //mutexWorkCompletion = PTHREAD_MUTEX_INITIALIZER; 
   
   pthread_mutex_lock(&mutexSync);
   this->maxThreads = maxThreads;
   this->queueSize = maxThreads;
   //workerQueue = new WorkerThread *[maxThreads];
   workerQueue.resize(maxThreads, NULL);
   topIndex = 0;
   bottomIndex = 0;
   incompleteWork = 0;
   sem_init(&availableWork, 0, 0);
   sem_init(&availableThreads, 0, queueSize);
   pthread_mutex_unlock(&mutexSync);
}

void ThreadPool::initializeThreads()
{
   for(int i = 0; i<maxThreads; ++i)
    {
        pthread_t tempThread;
        pthread_create(&tempThread, NULL, &ThreadPool::threadExecute, (void *) this ); 
         //threadIdVec[i] = tempThread;
   }

}

ThreadPool::~ThreadPool()
{
   workerQueue.clear();
}



void ThreadPool::destroyPool(int maxPollSecs = 2)
{
    while( incompleteWork>0 )
    {
            //cout << "Work is still incomplete=" << incompleteWork << endl;
        sleep(maxPollSecs);
    }
    cout << "Pool destroyed!" << endl;
    sem_destroy(&availableWork);
    sem_destroy(&availableThreads);
        pthread_mutex_destroy(&mutexSync);
        pthread_mutex_destroy(&mutexWorkCompletion);

}


bool ThreadPool::assignWork(WorkerThread *workerThread)
{
        pthread_mutex_lock(&mutexWorkCompletion);
        incompleteWork++;
        //cout << "assignWork...incomapleteWork=" << incompleteWork << endl;
    pthread_mutex_unlock(&mutexWorkCompletion);
    
    sem_wait(&availableThreads);
    
    pthread_mutex_lock(&mutexSync);
        //workerVec[topIndex] = workerThread;
        workerQueue[topIndex] = workerThread;
                //cout << "Assigning Worker[" << workerThread->id << "] Address:[" << workerThread << "] to Queue index [" << topIndex << "]" << endl;
        if(queueSize !=1 )
            topIndex = (topIndex+1) % (queueSize-1);
        sem_post(&availableWork);
    pthread_mutex_unlock(&mutexSync);
    return true;
}

bool ThreadPool::fetchWork(WorkerThread **workerArg)
{
    sem_wait(&availableWork);

    pthread_mutex_lock(&mutexSync);
        WorkerThread * workerThread = workerQueue[bottomIndex];
                workerQueue[bottomIndex] = NULL;
        *workerArg = workerThread;
        if(queueSize !=1 )
            bottomIndex = (bottomIndex+1) % (queueSize-1);
        sem_post(&availableThreads);
    pthread_mutex_unlock(&mutexSync);
    return true;
}

void *ThreadPool::threadExecute(void *param)
{
    WorkerThread *worker = NULL;
    
    while(((ThreadPool *)param)->fetchWork(&worker))
    {
        if(worker)
                {
            worker->executeThis();
                        //cout << "worker[" << worker->id << "]\tdelete address: [" << worker << "]" << endl;
                        delete worker;
                        worker = NULL;
                }

        pthread_mutex_lock( &(((ThreadPool *)param)->mutexWorkCompletion) );
                //cout << "Thread " << pthread_self() << " has completed a Job !" << endl;
         ((ThreadPool *)param)->incompleteWork--;
        pthread_mutex_unlock( &(((ThreadPool *)param)->mutexWorkCompletion) );
    }
    return 0;
}
