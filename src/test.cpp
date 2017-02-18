
#include "exploration.hpp"
#include "depth_first_search.hpp"
#include "dpor.hpp"
#include "sufficient_sets/persistent_set.hpp"
#include "sufficient_sets/source_set.hpp"
#include "sufficient_sets/bound_persistent_set.hpp"
#include "bound.hpp"
#include "bound_functions/preemptions.hpp"

using namespace bound_functions;
using namespace exploration;

// State-Space Exploration techniques

template<typename BoundFunction>
using bs = Exploration<depth_first_search<Bound<BoundFunction>>>;

template<typename SufficientSetImpl>
using dpor = Exploration<depth_first_search<DPOR<SufficientSetImpl>>>;

template<typename BoundFunction>
using bpor = Exploration<depth_first_search<DPOR<BoundPersistent<BoundFunction>>>>;

int main(int argc, char* argv[])
{
	const std::string mode = argv[1];
	const std::string dir = argv[2];
	const std::string program = argv[3];
	const unsigned int n = atoi(argv[4]);
	const unsigned int max = atoi(argv[5]);
        
	scheduler::Program B(dir, program, n);
	
	if (mode == "bs") {
		int nr_bs = 0;
		Preemptions::value_t bound = 0;
		bs<Preemptions> bs_(B,max,bound);
		while (true) {
			bs_.run();
		 	int nr = bs_.statistics().nr_explorations();
			if (nr <= nr_bs || nr == max) { break; }
			nr_bs = bs_.statistics().nr_explorations();
			++bound;
			bs_ = bs<Preemptions>(B,max,bound);
		}
	} else if (mode == "dpor_persistent") {
		dpor<Persistent>(B,max).run();
	} else if (mode == "dpor_source") {
		dpor<Source>(B,max).run();
	} else if (mode == "bpor") {
		int nr_bpor = 0;
		Preemptions::value_t bound = 0;
		while (true) {
			bpor<Preemptions> bpor_(B,max,bound);
			bpor_.run();
			int nr = bpor_.statistics().nr_explorations();
			if (nr <= nr_bpor || nr == max) { break; }
			nr_bpor = bpor_.statistics().nr_explorations();
			++bound;
		}
	}
    return 0;
}
