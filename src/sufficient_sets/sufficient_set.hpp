
#ifndef SUFFICIENT_SET_HPP_INCLUDED
#define SUFFICIENT_SET_HPP_INCLUDED

#include <iosfwd>
#include "sleep_set.hpp"
#include "thread.hpp"
// includes for the SufficientSet implementations
#include "color_output.hpp"
#include "debug.hpp"
#include "execution.hpp"
#include "happens_before.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file sufficient_set.hpp
 @brief Definition of class SufficientSet and struct backtrack_point.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace exploration
{
    /**
     @brief Subset of Tids whose exploration from the associated state s in the
	 exploration tree is sufficient to cover the space of behaviours reachable
	 from s. 
	 @details Implementation in terms of set mBacktrack and a SleepSet mSleepSet.
     */
    class SufficientSet
    {
    public:
        
        // CTORS
        
        SufficientSet();
        
        SufficientSet(const Tids& backtrack, const SleepSet& sleepset)
        : mBacktrack(backtrack)
        , mSleepSet(sleepset) { }
        
        //
        
        const Tids& backtrack() const;
        void add_to_backtrack(const Thread::tid_t&);
        void add_to_backtrack(const Tids&);
        
        SleepSet& sleepset();
        const SleepSet& sleepset() const;
        void set_sleepset(const SleepSet& sleepset);
        void wake_up(const Thread::tid_t&);
        
    private:
        
        // DATA MEMBERS
        
        Tids mBacktrack;
        SleepSet mSleepSet;
        
    }; // end class SufficientSet
    
    struct backtrack_point
    {
        Thread::tid_t tid;
        int index;
    };
    
    using BacktrackPoints = std::vector<backtrack_point>;
    
    std::ostream& operator<<(std::ostream&, const SufficientSet&);
    std::ostream& operator<<(std::ostream&, const backtrack_point&);
    
} // end namespace exploration

#endif
