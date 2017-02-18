
// EXPLORATION
#include "exploration.hpp"
#include "dependence.hpp"
#include "depth_first_search.hpp"
#include "dpor.hpp"
#include "happens_before.hpp"
#include "sufficient_sets/persistent_set.hpp"
#include "sufficient_sets/source_set.hpp"
#include "sufficient_sets/bound_persistent_set.hpp"

// STL
#include <iostream>

using namespace exploration;
template <typename sufficient_set_t>
using dpor_t = Exploration<depth_first_search<dpor<sufficient_set_t>>>;

int main(int argc, char* argv[])
{
	const std::string mode = argv[1];
	const std::string dir = argv[2];
	const std::string program_name = argv[3];
	const unsigned int nr_threads = atoi(argv[4]);
	const unsigned int max = atoi(argv[5]);
        
	scheduler::Program program(dir, program_name, nr_threads);

	if (mode == "persistent")
	{
		dpor_t<Persistent>(program, max).run();
		return 0;
	}
	else
	{
		std::cout << "mode has to be in { persistent }\n";
		return 1;
	}
}
