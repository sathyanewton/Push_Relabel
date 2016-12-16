#include <time.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <pthread.h>
#include <limits>
#include <atomic>

#define NoOfThreads 4

using namespace std;

/* Thread-based global variables   */
  pthread_t t[NoOfThreads];
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_id;



struct AtomicCounter {
    std::atomic<int> value;

    AtomicCounter() : value(0) {}

    void increment(){
        ++value;
    }

    void decrement(){
        --value;
    }

    int get(){
        return value.load();
    }
};

AtomicCounter counter,counter2;

void *thread(void *arg)
{


	pthread_mutex_lock(&mutex);
	thread_id++;
	//cout<<"printing thread id : "<<thread_id<<"\n";
	pthread_mutex_unlock(&mutex);

	counter.increment();
	counter2.increment();
	cout<<"counter value : "<<counter.get()<<"\n";
	cout<<"counter2 value : "<<counter2.get()<<"\n";

}




int main()
{
	int num_threads=0;
	int i,j,k;

	cout<<"enter number of threads to be spawned : ";
	cin>>num_threads;

	for(i=0;i<NoOfThreads;i++)
	{
		pthread_create(&t[i], NULL,thread,NULL);

	}
	for(i=0; i<NoOfThreads; i++)
  {
      pthread_join(t[i], NULL);
  }

return 0;
}