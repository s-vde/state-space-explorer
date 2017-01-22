
#ifndef SLEEPSETS_HPP_INCLUDED
#define SLEEPSETS_HPP_INCLUDED

#include "execution.hpp"
#include "non_preemptive.hpp"
#include "sleep_set.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file sleep_sets.hpp
 @brief Definition of class SleepSets.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace scheduler
{
	class SleepSets
	{
	public:
		
		using result_t = std::pair<Execution::Status,Thread::tid_t>;
		
		SleepSets();
		
		result_t select(const TaskPool& pool, const Tids& selection, const unsigned int task_nr);
		
	private:
		
		// DATA MEMBERS
		
		exploration::SleepSet mSleep;
		const exploration::Dependence mDep;
		NonPreemptive mAlternative;
		
		// HELPER FUNCTIONS
		
		void initialize();
		
		void update_sleep(const TaskPool& pool, const Instruction& current);
		
	}; // end class SleepSets
} // end namespace scheduler

#endif
