
// EXPLORATION
#include "exploration.hpp"
#include "depth_first_search.hpp"
#include "bound.hpp"
#include "bound_functions/preemptions.hpp"

// STL
#include <limits>

int main(int argc, char* argv[])
{
	const std::string dir = argv[1];
	const std::string program_name = argv[2];
	const unsigned int nr_threads = atoi(argv[3]);
	const unsigned int max = atoi(argv[4]);
        
	scheduler::Program program(dir, program_name, nr_threads);

	using namespace exploration;
	using dfs_t = Exploration<depth_first_search<bound<bound_functions::Preemptions>>>;
	
	dfs_t dfs(program, max, std::numeric_limits<int>::max());
	dfs.run();
	
   return 0;
}
