
#ifndef SLEEP_SET_HPP_INCLUDED
#define SLEEP_SET_HPP_INCLUDED

#include "container_output.hpp"
#include "debug.hpp"
#include "dependence.hpp"
#include "state.hpp"
#include "instruction.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file sleep_set.hpp
 @brief Definition of class SleepSet.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
   class Transition;
   
} // end namespace program_model

//----------------------------------------------------------------------------------------

using namespace program_model;

namespace exploration
{
    class SleepSet
    {
    public:
        
        // TYPES
        
        using transition = Transition;
        
        // CTORS
		
		/**
		 @brief Constructs an empty SleepSet.
		 */
        SleepSet();
        
        /**
         @brief Constructs a new SleepSet from a previous SleepSet and a 
         Transition t by propagating previous and waking-up sleeping Threads 
         according to dependencies with t.instr.
         */
		SleepSet(const SleepSet& previous, const transition& t, const Dependence& D);
		
        //
        
        /**
         @brief Removes { tid in mSleep | 
            pool.has_next(tid) && 
            D.dependent(instr, pool.next(tid)) 
         } from this SleepSet.
         */
        template<typename Pool, typename Retrieve>
        void wake_up(
            const Instruction& instr,
            const Pool& pool,
            const Retrieve retrieve,
            const Dependence& D)
        {
            // Cannot use std::remove_if on std::set.
            for (auto asleep = mSleep.begin(); asleep != mSleep.end(); ) {
                if (pool.has_next(*asleep) && D.dependent(instr, retrieve(pool, asleep))) {
                    DEBUG(*asleep << " ");
                    asleep = mSleep.erase(asleep);
                } else { ++asleep; }
            };
        }
        
        /**
         @brief Adds tid to mSleep.
         */
        void add(const Thread::tid_t& tid);
        
        /**
         @brief Removes tid from mSleep.
         */
        void wake_up(const Thread::tid_t& tid);
        
        /**
         @brief Returns { tid in Tids | tid notin mSleep }.
         */
        const Tids awake(const Tids&) const;
        
        /**
         @brief Returns true iff tid notin mSleep.
         */
        bool is_awake(const Thread::tid_t&) const;
        
    private:
        
        // DATA MEMBERS
		
        /// @brief The actual sleepset.
        Tids mSleep;

    //friendly:
        
        friend std::ostream& operator<<(std::ostream&, const SleepSet&);
        friend std::istream& operator>>(std::istream&, SleepSet&);
        
    }; // end class SleepSet

    std::ostream& operator<<(std::ostream&, const SleepSet&);
    std::istream& operator>>(std::istream&, SleepSet&);
    
} // end namespace exploration

#endif
