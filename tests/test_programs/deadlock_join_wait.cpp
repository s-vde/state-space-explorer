
#include <chrono>
#include <iostream>
#include <pthread.h>
#include <thread>

//--------------------------------------------------------------------------------------------------

class manager
{
public:
   manager() { pthread_mutex_init(&m_mutex, nullptr); }

   ~manager() { cancel_thread(); }

   void cancel_thread()
   {
      if (m_background_thread.joinable())
      {
         m_background_thread.join();
      }
   }

   void run_thread()
   {
      pthread_mutex_lock(&m_mutex);

      cancel_thread();
      std::cout << "[manager]\tcanceled background thread" << std::endl;

      m_background_thread = std::thread([this] {
         pthread_mutex_lock(&m_mutex);
         std::cout << "[background_thread]\tHello World!" << std::endl;
         pthread_mutex_unlock(&m_mutex);
         pthread_exit(0);
      });

      pthread_mutex_unlock(&m_mutex);
   }

private:
   std::thread m_background_thread;
   pthread_mutex_t m_mutex;

}; // end class manager

//--------------------------------------------------------------------------------------------------

int main()
{
   using namespace std::chrono_literals;

   manager m;
   m.run_thread();
   std::this_thread::sleep_for(1s);
   m.run_thread();

   return 0;
}