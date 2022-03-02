#include "mylibthread_POSIX.h"

pthread_t getTid(){
	return pthread_self();
}

//merge of the two setcancel without oldtype
void setcancel(int state, int type){
	pthread_setcancelstate(state, NULL);
	pthread_setcanceltype(type, NULL);
}


////////////////////////////////
//////////// MUTEX /////////////
////////////////////////////////

int mLock(pthread_mutex_t *m){
	return pthread_mutex_lock(m);
}

int mTryLock(pthread_mutex_t *m){
	return pthread_mutex_trylock(m);
}

int mUnLock(pthread_mutex_t *m){
	return pthread_mutex_unlock(m);
}

int mDestroy(pthread_mutex_t *m){
	return pthread_mutex_destroy(m);
}

int mInitDef(pthread_mutex_t *m){
	return pthread_mutex_init(m, NULL);
}


////////////////////////////////
////////// CONDITION ///////////
////////////////////////////////

int condInitDef(pthread_cond_t *cond){
	return pthread_cond_init(cond, NULL);
}

int condWait(pthread_cond_t *cond, pthread_mutex_t *mutex){
	return pthread_cond_wait(cond, mutex);
}

int condSig(pthread_cond_t *cond){
	return pthread_cond_signal(cond);
}


////////////////////////////////
///////// SPECIFIC VAR /////////
////////////////////////////////

int setSpec(pthread_key_t cle,const void* value){
	return pthread_setspecific(cle,value);
}

void* getSpec(pthread_key_t cle){
	return pthread_getspecific(cle);
}