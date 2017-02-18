
#include "exploration.hpp"
#include "depth_first_search.hpp"
#include "dpor.hpp"
#include "bound.hpp"
#include "bound_functions/preemptions.hpp"

namespace exploration 
{
	template <typename bound_function_t>
	using bounded_search = Exploration<depth_first_search<bound<bound_function_t>>>;
}

int main(int argc, char* argv[])
{
	const std::string bound_function = argv[1];
	const unsigned int bound_value = atoi(argv[2]);
	const std::string dir = argv[3];
	const std::string program_name = argv[4];
	const unsigned int nr_threads = atoi(argv[5]);
	const unsigned int max = atoi(argv[6]);
        
	scheduler::Program program(dir, program_name, nr_threads);
	
	if (bound_function == "preemptions")
	{
		using namespace bound_functions;
		exploration::bounded_search<Preemptions> bs(program, max, bound_value);
		bs.run();
		return 0;
	}
	else
	{
		std::cout << "bound_function has to be in { preemptions }\n";
		return 1;
	}
}
