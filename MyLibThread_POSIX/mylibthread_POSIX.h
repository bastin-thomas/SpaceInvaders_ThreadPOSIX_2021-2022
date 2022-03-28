//mylibthread_POSIX.h
#ifndef MYLIBTHREAD_POSIX_H
#define MYLIBTHREAD_POSIX_H

//Posix thread Lib
#include <pthread.h>

//Signal Linux Lib
#include <signal.h>

//Standard Library 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


//Replace of pthread Self
pthread_t getTid();

//merge of the two setcancel without oldtype
void setcancel(int state, int type);



////////////////////////////////
//////////// MUTEX /////////////
////////////////////////////////

//Replace of Pthread MutexInit
int mInitDef(pthread_mutex_t *m);

//Replace of Pthread MutexLock
int mLock(pthread_mutex_t *m);

//Replace of Pthread MutexTryLock
int mTryLock(pthread_mutex_t *m);

//Replace of Pthread MutexUnLock
int mUnLock(pthread_mutex_t *m);

//Replace of Pthread MutexDestroy
int mDestroy(pthread_mutex_t *m);



////////////////////////////////
////////// CONDITION ///////////
////////////////////////////////

//Replace of Pthread Init
int condInitDef(pthread_cond_t *cond);

//Replace of Pthread Cond Wait
int condWait(pthread_cond_t *cond, pthread_mutex_t *mutex);

//Replace of Pthread Cond Signal
int condSig(pthread_cond_t *cond);


////////////////////////////////
///////// SPECIFIC VAR /////////
////////////////////////////////

int setSpec(pthread_key_t cle,const void* value);

void* getSpec(pthread_key_t cle);

#endif