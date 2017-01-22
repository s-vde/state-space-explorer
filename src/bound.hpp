
#ifndef BOUND_HPP_INCLUDED
#define BOUND_HPP_INCLUDED

#include "debug.hpp"
#include "container_output.hpp"
#include "execution.hpp"
#include "instruction_io.hpp"
#include "transition_io.hpp"
#include "scheduler_settings.hpp"
#include "state.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file bound.hpp
 @brief Definition of class BoundState and class template Bound<BoundFunction>.
 @author Susanne van den Elsen
 @date 2015-2016
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace exploration
{
    class BoundState
    {
    public:
        
        // CTOR
        
        explicit BoundState(const int boundvalue=0);
		
        //
        
        int boundvalue() const;
        void set_boundvalue(const int bound_value);
        
        std::string to_string() const;
        
    private:
        
        // DATA MEMBERS
        
        int mBoundValue;
        
    }; // end class BoundState
    
    std::ostream& operator<<(std::ostream&, const BoundState&);
	
	template<typename BoundFunction>
    class Bound
    {
    public:
        
        // TYPES
        
        using execution = Execution;
       using transition = typename Execution::transition_t;
        
        // CTOR
		
		explicit Bound(const execution& E, const typename BoundFunction::value_t bound)
        : mState({ BoundState() })
        , mBound(bound) { }
		
		//
        
        static std::string name()
        {
            return "Bound";
        }
        
        std::string path() const
        {
            std::string path = name();
            path += "/";
            path += BoundFunction::name();
            path += "/";
            path += std::to_string(mBound);
            return path;
        }
        
        /**
         Returns default SchedulerSettings.
         */
        scheduler::SchedulerSettings scheduler_settings()
        {
            return scheduler::SchedulerSettings();
        }
        
        /**
         @brief Does nothing.
         */
        void write_scheduler_files() { }
		
		/**
		 @brief Returns true.
		 */
		bool check_valid(const bool contains_locks) const
		{
			return true;
		}
        
        /**
         @brief Does nothing.
         */
        void reset() { }
        
        /**
         @brief Does nothing.
         */
        void update_statistics(const execution&) { }
        
        /**
         @brief Does nothing.
         */
        void restore_state(const transition& t) { }

        /**
         @detail Adds a new BoundState to mState with a boundvalue that is 
         the sum of the boundvalue corresponding to E[t.index()-1] and the
         step value of E[t.index()-1] and t defined by BoundFunction.
         @note Assumes that BoundFunction is local (i.e. the step_value can 
         be determined using only E[t.index()-1] and t.
         */
        void update_state(const execution& E, const transition& t)
        {
            /// @pre mState.size() == t.index()
            assert(mState.size() == t.index());
            mState.emplace_back(
                BoundFunction::value(
                    E,
                    mState,
                    t.index()-1,
                    t.instr().tid()
                )
            );
            DEBUGFNL(
                outputname(), "update_state", t.instr(),
                to_string_post_state(t) << ".boundvalue = " << mState.back().boundvalue()
            );
        }
        
        /**
         @brief Does nothing.
         */
        void update_after_exploration(transition& t) { }
        
        /**
         @note Returns a subset of E.final().enabled.
         */
        Tids pool(const execution& E)
        {
            Tids Pool{};
            std::copy_if(
                E.final().enabled().begin(), E.final().enabled().end(),
                std::inserter(Pool, Pool.end()),
                [this, &E] (const auto& tid) {
                    return BoundFunction::value(E, mState, E.size(), tid) <= mBound;
                }
            );
            return Pool;
        }
        
        /**
         @brief Returns the first tid in the pool.
         */
        static Thread::tid_t select_from_pool(const execution& E, const Tids& pool)
        {
            /// @pre !pool.empty()
            assert(!pool.empty());
            DEBUGFNL(outputname(), "select_from_pool", pool, *(pool.begin()));
            return *(pool.begin());
        }
        
        void pop_back()
        {
            mState.pop_back();
        }
        
        /**
         @brief Does nothing.
         */
        void close(const std::string& statistics) const { }
        
        // OUTPUT
        
        void dump_state(std::ostream& os, const unsigned int i) const
        {
            os << mState[i];
        }
        
        // DEBUGGING
        
        static std::string full_name()
        {
            std::string full_name = name();
            full_name += "<";
            full_name += BoundFunction::name();
            full_name += ">";
            return full_name;
        }
        
    private:
        
        // DATA MEMBERS
        
        std::vector<BoundState> mState;
        int mBound;
        
        // DEBUGGING
        
        static std::string tabs()
        {
            return "\t\t";
        }
        
        static std::string outputname()
        {
            std::string outputname = tabs();
            outputname += name();
            return outputname;
        }
    }; // end class template Bound<BoundFunction>
} // end namespace exploration

#endif
