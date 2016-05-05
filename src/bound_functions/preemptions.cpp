
#include "preemptions.hpp"

namespace bound_functions
{
    std::string Preemptions::name()
    {
        return "Preemptions";
    }
	
	bool Preemptions::context_switch(const Thread::tid_t& tid1, const Thread::tid_t& tid2)
	{
		return tid1 != tid2;
	}
} // end namespace bound_functions
