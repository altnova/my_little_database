//! \file thr.c \brief threads

#include "../___.h"
#include "pth.h"

#define CN 4

Z pthread_attr_t p;
Z pthread_rwlock_t l;
Z pthread_mutexattr_t mattr;
Z pthread_mutex_t cs[CN];
Z UJ stat[CN];
Z pthread_t ids[CN];

#ifdef __APPLE__
typedef semaphore_t D;
//ZV h0(D*d){semaphore_signal(*d);}
//ZV h1(D*d){semaphore_wait(*d);}
//ZV h2(D*d){semaphore_create(mach_task_self(), d, SYNC_POLICY_FIFO, 0);}
#else
typedef sem_t D;
ZV h0(D*d){sem_post(d);}
ZV h1(D*d){W(-1==sem_wait(d));}
ZV h2(D*d){sem_init(d,0,0);}
#endif

Z pthread_t thr_id(){R pthread_self();}
ZV c1(I i){pthread_mutex_lock(cs+i);}     //!< enter critical section
ZV c0(I i){pthread_mutex_unlock(cs+i);}   //!< leave critical section

Z pthread_t t1(V*(*fn)(V*),V*v){
	LOG("t1");pthread_t t;
	I r = pthread_create(&t,&p,fn,v);
	if(r){T(WARN,"pthread_create %d",r);}
	R t;}

I __thread w;
V* pth_worker(V*a){
	LOG("pth_worker");I d;
	DO(CN,if((I)thr_id()==(I)ids[i]){d=i;break;})
	T(WARN, "%ld: arg %d", thr_id(), *(I*)a);//d
	UJ n = 0;
	W(1) {
		n++;
		sleep(1);
		stat[d] = n;
	}
	R (V*)&w;}

pthread_t pth_fork(I d){
	R t1(pth_worker,&d);}

V pth_join(pthread_t t){
	I r = pthread_join(t, NULL);
	O("thread joined res=%d\n",r);}

I main() {
	pthread_rwlock_init(&l, NULL);
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr,PTHREAD_MUTEX_RECURSIVE);
	DO(CN,(pthread_mutex_init(cs+i,&mattr)))
	pthread_attr_init(&p),
	pthread_attr_setdetachstate(&p,PTHREAD_CREATE_DETACHED);

	DO(4,ids[i]=pth_fork(i))

	W(1){
		sleep(3);
		DO(4,O("%lu ", stat[i]));O("\n");
		//DO(4,pth_join(ids[i]))
	}
	//DO(4,pth_join(ids[i]))
}