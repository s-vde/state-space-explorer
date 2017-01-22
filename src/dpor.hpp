
#ifndef DPOR_HPP_INCLUDED
#define DPOR_HPP_INCLUDED

#include "execution.hpp"
#include "scheduler_settings.hpp"
#include "sufficient_sets/sufficient_set.hpp"
#include "transition_io.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file dpor.hpp
 @brief Definition of classes DPORStatistics and DPORBase and class template 
 DPOR<SufficientSetImpl>.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace exploration
{
	using namespace program_model;
	
    class DPORStatistics
    {
    public:
        
        // CTOR
        
        DPORStatistics();
		
        //
        
        unsigned int nr_sleepset_blocked() const;
        void increase_nr_sleepset_blocked();
        
        void dump(const std::string& filename) const;

    private:
        
        // DATA MEMBERS
        
        unsigned int mNrSleepSetBlocked;
        
    }; // end class DPORStatistics
    
    std::ostream& operator<<(std::ostream&, const DPORStatistics&);
	
	class DPORBase
	{
	public:
		
		// TYPES
		
		using execution = Execution;
      using transition = typename Execution::transition_t;
		
		// CTOR
		
		explicit DPORBase(const execution&);
		
		//
		
		/**
		 @brief Sets the selection strategy to SleepSets.
		 */
		static scheduler::SchedulerSettings scheduler_settings();
		
		/**
		 @brief Writes the sleepset of mState.back() to file
		 schedules/sleepset.txt.
		 */
		void write_scheduler_files();
		
		/**
		 @brief Wrapper of mHB.reset.
		 */
		void reset();
		
		/**
		 @brief Wrapper of mHB.restore, checking preconditions.
		 */
		void restore_state(const transition& t);
		
		/**
		 @brief Calls mStatistics.increase_nr_sleepset_blocked iff E.status
		 is BLOCKED.
		 */
		void update_statistics(const execution& E);
		
		void close(const std::string& statistics) const;
		
	protected:
		
		// DATA MEMBERS
		
		std::vector<SufficientSet> mState;
		HappensBefore<Dependence> mHB;
		DPORStatistics mStatistics;
		
		//
		
		/**
		 @details Adds t.instr.tid() to t.pre.backtrack(), propagates the
		 SleepSet from t.post to t.pre, and sets t.post.clock
		 @note It is important for SleepSet propagation that this function
		 is only called on *new* Transitions (i.e. index >= from), because
		 otherwise SleepSets are overridden during propagation.
		 */
		void update_state(const execution& E, const transition& t);
		
		SufficientSet& pre_of_transition(const unsigned int t_i);
		
		static const std::string name;
		static const std::string tabs;
		static std::string outputname();
		
	}; // end class DPORBase
	
    /**
     @brief Implements partial-order reduction based on SufficientSetImpl, 
	 to be used with DFS.
	 
     @details It adds a happens-before relation on an Execution object. 
     The argument to template paramter SufficientSetImpl determines - based on 
     the happens-before relation - the set of Thread::tid_t's that is 
     sufficient to explore from each State in the exploration tree to obtain 
     the level of completeness associated with it.
     */
    template<typename SufficientSetImpl> class DPOR : public DPORBase
    {
    public:
        
        // CTOR
		
		template<typename ... Args>
        explicit DPOR(const execution& E, Args ... args)
        : DPORBase(E)
		, mSufficientSet(std::forward<Args>(args) ...) { }
        
        //
		
		/**
		 @brief Wrapper for mSufficientSet::check_valid.
		 */
		bool check_valid(const bool contains_locks) const
		{
			return mSufficientSet.check_valid(contains_locks);
		}
        
        std::string path() const
        {
            std::string path = name;
            path += "/";
            path += mSufficientSet.path();
            return path;
		}
		
        /**
         @details Calls DPORBase::update_state and uses the functionality of 
		 SufficientSetImpl to add backtrack points.
         */
        void update_state(const execution& E, const transition& t)
        {
			DPORBase::update_state(E, t);
			mSufficientSet.update_state(E, t);
            BacktrackPoints Points = mSufficientSet.backtrack_points(E, t.index(), mHB);
            DEBUGNL(tabs << "\tBacktrackPoints = " << Points);
            for (const auto& point : Points) {
                mSufficientSet.add_backtrack_point(E, t.index(), mState, mHB, point);
                DEBUGNL(tabs << "\t" << to_string_pre(point.index) << ".backtrack = " << pre_of_transition(point.index).backtrack());
            }
        }
        
        /**
         @details Adds t.instr.tid to t.pre.sleepset and calls
         SufficientSetImpl::update_after_exploration.
         */
        void update_after_exploration(transition& t)
        {
            /// @pre mState.size() == t.index()+1 (i.e. did not yet pop_back())
            assert(mState.size() == t.index()+1);
            mState[t.index()-1].sleepset().add(t.instr().tid());
            DEBUGFNL(
                outputname(), "update_after_exploration", to_string_pre_state(t),
                to_string_pre_state(t) << ".sleep += { " << t.instr().tid() << " } = "
                << mState.back().sleepset()
            );
            mSufficientSet.update_after_exploration(t, mState[t.index()-1]);
        }
        
        /**
         @details Returns state.sleepset.awake(state.backtrack \ state.done)
         if sleepsets are enabled and state.backtrack \ state.done otherwise.
         */
        Tids pool(const execution&)
        {
            SufficientSet& s = mState.back();
			Tids Sufficient = s.sleepset().awake(s.backtrack());
            mSufficientSet.add_to_pool(Sufficient);
            return Sufficient;
        }
        
        /**
         @brief Selects the first Thread::tid_t from the given pool that
         satisfies the condition set by SufficientSetImpl. If no such
         Thread::tid_t is found, it returns -1.
         */
        Thread::tid_t select_from_pool(const execution& E, const Tids& pool)
        {
            /// @pre !pool.empty()
            assert(!pool.empty());
            DEBUGFNL(outputname(), "select_from_pool", pool, "");
            auto it = std::find_if(
                pool.begin(), pool.end(),
                [this, &E] (const auto& tid) {
                    return mSufficientSet.condition(E, mState.back(), tid);
                }
            );
            return (it != pool.end()) ? *it : -1;
        }
        
        void pop_back()
        {
            mState.pop_back();
            mHB.pop_back();
            mSufficientSet.pop_back();
        }
        
        // OUTPUT
        
        void dump_state(std::ostream& os, const unsigned int i) const
        {
            os << mState[i];
        }
        
        // DEBUGGING
        
        static std::string full_name()
        {
            std::string full_name = name;
            full_name += "<";
            full_name += SufficientSetImpl::name();
            full_name += ">";
            return full_name;
        }
		
    private:
		
        // DATA MEMBERS
		
        SufficientSetImpl mSufficientSet;
		
    }; // end class template DPOR<SufficientSetImpl>
} // end namespace exploration

#endif
