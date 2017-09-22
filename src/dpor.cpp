
#include "dpor.hpp"

// UTILS
#include "utils_io.hpp"

namespace exploration
{
//--------------------------------------------------------------------------------------------------

dpor_statistics::dpor_statistics()
: mNrSleepSetBlocked(0) { }

//--------------------------------------------------------------------------------------------------
    
unsigned int dpor_statistics::nr_sleepset_blocked() const
{
   return mNrSleepSetBlocked;
}

//--------------------------------------------------------------------------------------------------
    
void dpor_statistics::increase_nr_sleepset_blocked()
{
   ++mNrSleepSetBlocked;
}

//--------------------------------------------------------------------------------------------------
    
std::ostream& operator<<(std::ostream& os, const dpor_statistics& stats)
{
   os << "nr_sleepset_blocked\t" << stats.nr_sleepset_blocked() << std::endl;
   return os;
}

//--------------------------------------------------------------------------------------------------
	
dpor_base::dpor_base(const execution_t& execution) 
: mState({ SufficientSet() })
, mHB(execution) 
{ 
}

//--------------------------------------------------------------------------------------------------
	
scheduler::SchedulerSettings dpor_base::scheduler_settings()
{
	return scheduler::SchedulerSettings("SleepSets");
}

//--------------------------------------------------------------------------------------------------
	
void dpor_base::write_scheduler_files() const
{
	/// @pre !mState.empty()
	assert(!mState.empty());
	utils::io::write_to_file("schedules/sleepset.txt", mState.back().sleepset());
}

//--------------------------------------------------------------------------------------------------
	
void dpor_base::reset()
{
	mHB.reset();
}

//--------------------------------------------------------------------------------------------------
	
void dpor_base::restore_state(const transition_t& transition)
{
	/// @pre mState.size() > transition.index()
	assert(mState.size() > transition.index());
	mHB.restore(transition.index());
}

//--------------------------------------------------------------------------------------------------
	
void dpor_base::update_statistics(const execution_t& execution)
{
	if (execution.status() == execution_t::Status::BLOCKED) {
		mStatistics.increase_nr_sleepset_blocked();
	}
}

//--------------------------------------------------------------------------------------------------
	
void dpor_base::close(const std::string& statistics_file) const
{
   utils::io::write_to_file(statistics_file, mStatistics, std::ios::app);
}

//--------------------------------------------------------------------------------------------------
	
void dpor_base::update_state(const execution_t& execution, const transition_t& transition)
{
	/// @pre mState.size() == transition.index()
	assert(mState.size() == transition.index());
   const auto tid = boost::apply_visitor(program_model::get_tid(), transition.instr());
   mState.back().add_to_backtrack(tid);
	mState.emplace_back(SufficientSet{{}, SleepSet(mState.back().sleepset(), transition, Dependence())});
	mHB.update(transition.index());
	/// @post mState.size() == transition.index()+1
	assert(mState.size() == transition.index()+1);
}

//--------------------------------------------------------------------------------------------------
	
SufficientSet& dpor_base::pre_of_transition(const std::size_t index)
{
	return mState[index-1];
}

//--------------------------------------------------------------------------------------------------
	
const std::string dpor_base::name = "dpor";

//--------------------------------------------------------------------------------------------------
	
std::string dpor_base::outputname()
{
	return text_color(name, utils::io::Color::CYAN);
}

//--------------------------------------------------------------------------------------------------
} // end namespace exploration