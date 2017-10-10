
//--------------------------------------------------------------------------------------------------
/// @file readers-nonpreemptive.c
/// @brief Example program from @cite Abdulla:2014:ODP:2535838.2535845
/// demonstrating the advantage of DPOR<Source> over DPOR<Persistent>.
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------

#include <pthread.h>

#ifndef NR_THREADS
#define NR_THREADS 3
#endif

//--------------------------------------------------------------------------------------------------

int x[NR_THREADS];

//--------------------------------------------------------------------------------------------------

void* writer(void* arg)
{
   x[0] = 1;
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

void* reader(void* arg)
{
   int id = *(int*)arg;
   int local = x[id];
   local = x[0];
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

int main()
{
   pthread_t threads[NR_THREADS];
   int tids[NR_THREADS];
	
	// Initialize global array x
	for (int i = 0; i < NR_THREADS; ++i)
	{
		x[i] = 0;
	}
	
	// Create writer thread
	pthread_create(threads + 0, NULL, writer, NULL);
	
	// Create reader threads
   for (int i = 1; i < NR_THREADS; ++i)
   {
      tids[i] = i;
      pthread_create(threads + i, NULL, reader, tids + i);
   }
	
	// Join threads
   for (int i = 0; i < NR_THREADS; ++i)
   {
      pthread_join(threads[i], NULL);
   }
	
   return 0;
}
