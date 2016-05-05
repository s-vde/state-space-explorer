
#include "sleep_set.hpp"
#include "container_io.hpp"
#include "utils_io.hpp"

namespace exploration
{
    SleepSet::SleepSet()
    : mSleep() { }
	
	SleepSet::SleepSet(const SleepSet& previous, const transition& t, const Dependence& D)
	: mSleep(previous.mSleep)
	{
		DEBUGF("\t\t\tSleepSet", "propagate_and_wake_up", to_short_string(t), "");
		DEBUG("mSleep = " << previous << " \\ { ");
		wake_up(
			t.instr(),
			t.post(),
			[] (const auto& pool, const auto& asleep) {
				return pool.next(*asleep)->second.instr; },
			D
		);
		DEBUGNL("} = " << mSleep);
	}
	
    void SleepSet::add(const Thread::tid_t& tid)
    {
        mSleep.insert(tid);
    }
	
    void SleepSet::wake_up(const Thread::tid_t& tid)
    {
        mSleep.erase(tid);
    }
    
    const Tids SleepSet::awake(const Tids& tids) const
    {
        Tids awake{};
        std::set_difference(
            tids.begin(), tids.end(),
            mSleep.begin(), mSleep.end(),
            std::inserter(awake, awake.begin())
        );
        return awake;
    }
    
    bool SleepSet::is_awake(const Thread::tid_t& tid) const
    {
        return mSleep.find(tid) == mSleep.end();
    }
    
    std::ostream& operator<<(std::ostream& os, const SleepSet& sleep)
    {
        os << sleep.mSleep;
        return os;
    }
    
    std::istream& operator>>(std::istream& is, SleepSet& sleep)
    {
        is >> sleep.mSleep;
        return is;
    }
    
} // end namespace exploration
