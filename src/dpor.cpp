
#include "dpor.hpp"
#include "utils_io.hpp"

namespace exploration
{
    DPORStatistics::DPORStatistics()
    : mNrSleepSetBlocked(0) { }
    
    unsigned int DPORStatistics::nr_sleepset_blocked() const
    {
        return mNrSleepSetBlocked;
    }
    
    void DPORStatistics::increase_nr_sleepset_blocked()
    {
        ++mNrSleepSetBlocked;
    }
    
    void DPORStatistics::dump(const std::string& filename) const
    {
        utils::io::write_to_file(filename, *this, std::ios::app);
    }
    
    std::ostream& operator<<(std::ostream& os, const DPORStatistics& stats)
    {
        os << "nr_sleepset_blocked\t" << stats.nr_sleepset_blocked() << std::endl;
        return os;
    }
	
	DPORBase::DPORBase(const execution& E)
	: mState({ SufficientSet() })
	, mHB(E)
	, mStatistics() { }
	
	scheduler::SchedulerSettings DPORBase::scheduler_settings()
	{
		return scheduler::SchedulerSettings("SleepSets");
	}
	
	void DPORBase::write_scheduler_files()
	{
		/// @pre !mState.empty()
		assert(!mState.empty());
		utils::io::write_to_file("schedules/sleepset.txt", mState.back().sleepset());
	}
	
	void DPORBase::reset()
	{
		mHB.reset();
	}
	
	void DPORBase::restore_state(const transition& t)
	{
		/// @pre mState.size() > t.index()
		assert(mState.size() > t.index());
		mHB.restore(t.index());
	}
	
	void DPORBase::update_statistics(const execution& E)
	{
		if (E.status() == execution::Status::BLOCKED) {
			mStatistics.increase_nr_sleepset_blocked();
		}
	}
	
	void DPORBase::close(const std::string& statistics) const
	{
		mStatistics.dump(statistics);
	}
	
	void DPORBase::update_state(const execution& E, const transition& t)
	{
		/// @pre mState.size() == t.index()
		assert(mState.size() == t.index());
		DEBUGFNL(outputname(), "update_state", to_short_string(t), "");
		mState.back().add_to_backtrack(t.instr().tid());
		mState.emplace_back(SufficientSet{
			{}, SleepSet(mState.back().sleepset(), t, Dependence())
		});
		mHB.update(t.index());
		/// @post mState.size() == t.index()+1
		assert(mState.size() == t.index()+1);
	}
	
	SufficientSet& DPORBase::pre_of_transition(const unsigned int t_i)
	{
		return mState[t_i-1];
	}
	
	const std::string DPORBase::name = "DPOR";
	
	const std::string DPORBase::tabs = "\t\t";
	
	std::string DPORBase::outputname()
	{
		std::string outputname = tabs;
		outputname += text_color(name, utils::io::Color::CYAN);
		return outputname;
	}
	
} // end namespace exploration