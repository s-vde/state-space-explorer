
#include <thread>

//--------------------------------------------------------------------------------------------------
/// @brief In this test program, two threads modify the same shared variable,
/// but not concurrently.
//--------------------------------------------------------------------------------------------------


int main()
{
   int shared_variable = 0;

   std::thread spawn_thread([&shared_variable]() {
      shared_variable = 1;
   });
   spawn_thread.join();
   shared_variable = 2;
}
