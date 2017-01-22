
#include "sleep_sets.hpp"
#include <instruction_io.hpp>
#include <assert.h>
#include <fstream>
#include "container_output.hpp"

namespace scheduler
{
	SleepSets::SleepSets()
	: mSleep()
	, mDep()
	, mAlternative()
	{
		initialize();
	}

	/**
	 @brief Updates mSleepSet, calls NonPreemptive::select on the subset of 
	 the given selection that is awake, if this subset is non-empty. 
	 Otherwise, returns (BLOCKED,-1).
	 */
	SleepSets::result_t SleepSets::select(
		const TaskPool& pool, const Tids& selection, const unsigned int task_nr)
	{
		/// @pre !selection.empty
		assert(!selection.empty());
		if (task_nr > 0) {
			update_sleep(pool, *pool.current_task());
		}
		const Tids Awake = mSleep.awake(selection);
		DEBUGF("SleepSets", "select", "", selection << " \\ " << mSleep << " = " << Awake);
		if (!Awake.empty()) { return mAlternative.select(pool, Awake, task_nr);		}
		else                { return result_t(Execution::Status::BLOCKED, -1);	}
	}
	
	void SleepSets::initialize()
	{
		std::ifstream ifs("schedules/sleepset.txt");
		ifs >> mSleep;
		ifs.close();
		DEBUGFNL("SleepSets", "initialize", "", "mSleep = " << mSleep);
	}
	
	void SleepSets::update_sleep(const TaskPool& pool, const Instruction& current)
	{
		DEBUGF("SleepSets", "update_sleep", current, "");
		mSleep.wake_up(
        	current,
        	pool,
        	[] (const auto& pool, const auto& asleep) {
				return pool.task(*asleep)->second;
			},
        	mDep
		);
	}
} // end namespace scheduler
