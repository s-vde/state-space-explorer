
#include "sufficient_set.hpp"
#include <iostream>

namespace exploration
{	
    SufficientSet::SufficientSet()
    : mBacktrack()
    , mSleepSet() { }
    
    const Tids& SufficientSet::backtrack() const
    {
        return mBacktrack;
    }
    
    void SufficientSet::add_to_backtrack(const Thread::tid_t& tid)
    {
        mBacktrack.insert(tid);
    }
    
    void SufficientSet::add_to_backtrack(const Tids& tids)
    {
        std::set_union(
            mBacktrack.begin(), mBacktrack.end(),
            tids.begin(), tids.end(),
            std::inserter(mBacktrack, mBacktrack.begin())
        );
    }
    
    SleepSet& SufficientSet::sleepset()
    {
        return mSleepSet;
    }
    
    const SleepSet& SufficientSet::sleepset() const
    {
        return mSleepSet;
    }
    
    void SufficientSet::set_sleepset(const SleepSet& sleepset)
    {
        mSleepSet = sleepset;
    }
    
    void SufficientSet::wake_up(const Thread::tid_t& tid)
    {
        mSleepSet.wake_up(tid);
    }

    std::ostream& operator<<(std::ostream& os, const SufficientSet& s)
    {
        os  << "backtrack=" << s.backtrack()
            << " sleep=" << s.sleepset();
        return os;
    }
    
    std::ostream& operator<<(std::ostream& os, const backtrack_point& point)
    {
        os << "(tid=" << point.tid << ", index=" << point.index << ")";
        return os;
    }
} // end namespace exploration