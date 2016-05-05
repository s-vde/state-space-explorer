
#ifndef DFS_HPP_INCLUDED
#define DFS_HPP_INCLUDED

#include "color_output.hpp"
#include "container_output.hpp"
#include "debug.hpp"
#include "execution.hpp"
#include "schedule.hpp"
#include "scheduler_settings.hpp"
#include "state.hpp"
#include "transition_io.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file dfs.hpp
 @brief Definition of class DFSState and class template DFS<Reduction>.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace exploration
{
    class DFSState
    {
    public:
		
        DFSState();
        
        void add_to_done(const Thread::tid_t& tid);
        Tids undone(const Tids& T) const;
        
    private:
        
        Tids mDone;
        
    //friendly:
        
        friend std::ostream& operator<<(std::ostream&, const DFSState&);
        
    }; // end class DFSState
    
    std::ostream& operator<<(std::ostream&, const DFSState&);
  
    /**
     @brief Implements a depth-first traversal of the state-space, by treating 
	 the Execution object as a stack.
	 
     @details The argument to template parameter Reduction is used to 
     (potentially) reduce the exploration tree by restricting the pool of 
     Thread::tid_t's to explore in each internal node.
     */
    template<typename Reduction>
	class DFS
    {
    public:
        
        // TYPES

        using execution = Execution<State>;
        using transition = Transition<State>;
        
        // CTOR
		
		template<typename ... Args>
		explicit DFS(const execution& E, Args ... args)
        : mState({ DFSState() })
		, mReduction(E, std::forward<Args>(args) ...) { }
		
        //
        
        static std::string name()
        {
            return "DFS";
        }
        
        std::string path() const
        {
            std::string path = name();
            path += "/";
            path += mReduction.path();
            return path;
        }
        
        /**
         @brief Wrapper of mReduction.scheduler_settings.
         */
        scheduler::SchedulerSettings scheduler_settings()
        {
            return mReduction.scheduler_settings();
        }
        
        /**
         @brief Wrapper of mReduction.write_scheduler_files.
         */
        void write_scheduler_files()
        {
            mReduction.write_scheduler_files();
        }
		
		/**
		 @brief Wrapper of mReduction.check_valid.
		 */
		bool check_valid(const bool contains_locks) const
		{
			return mReduction.check_valid(contains_locks);
		}
        
        /**
         @brief Wrapper of mReduction.update_statistics.
         */
        void update_statistics(const execution& e)
        {
            mReduction.update_statistics(e);
        }
        
        /**
         @brief Wrapper of mReduction.restore_state, checking preconditions.
         */
        void restore_state(const transition& t)
        {
            /// @pre mState.size() > t.index()
            assert(mState.size() > t.index());
            mReduction.restore_state(t);
        }
        
        /**
         @brief Adds a new DFSState at the back of mState and calls 
         mReduction.update_state.
         */
        void update_state(const execution& E, const transition& t)
        {
            /// @pre mState.size() == t.index()
            assert(mState.size() == t.index());
            DEBUGFNL(outputname(), "update_state", to_short_string(t), "");
            mState.emplace_back();
            mReduction.update_state(E, t);
            /// @post mState.size() == t.index()+1
            assert(mState.size() == t.index()+1);
        }
        
        /**
         @brief Adds t.instr to mState[t.index-1].pre.done and calls 
         mReduction.update_after_exploration.
         */
        void update_after_exploration(transition& t)
        {
            /// @pre mState.size() == t.index()+1 (i.e. did not yet pop_back())
            assert(mState.size() == t.index()+1);
            DEBUGFNL(outputname(), "update_after_exploration", to_short_string(t), "");
            DEBUGNL(tabs() << "\t" << to_string_pre_state(t) << ".done.add(" << t.instr().tid() << ")");
            mState[t.index()-1].add_to_done(t.instr().tid()); // t.pre
            mReduction.update_after_exploration(t);
        }
        
        /**
         @brief Wrapper of mReduction.reset.
         */
        void reset()
        {
            mReduction.reset();
        }
        
        /**
         @brief Backtracks the states along the current execution until it
         encounters i such that exists tid in (mMode.pool(i) \ mState[i-1].done()).
         Then it returns the new schedule pre(E, i).tid.
         */
        scheduler::schedule_t new_schedule(execution& E, scheduler::schedule_t& S)
        {
            DEBUGFNL(outputname(), "NEW_SCHEDULE", "", "");
            while (!E.empty()) {
                update_after_exploration(E.last());
                pop_back(E, S);
				DEBUGNL(E.final());
                Tids PoolUndone = mState.back().undone(mReduction.pool(E));
                if (!PoolUndone.empty()) {
                    const Thread::tid_t next = mReduction.select_from_pool(E, PoolUndone);
                    if (next >= 0) {
                        DEBUGNL(tabs() << "\t= " << next);
                        /// @invariant E.final().is_enabled(mReduction.select_from_pool(E, PoolUndone))
                        /// (i.e. mReduction.pool(E) is a subset of E.final().enabled())
                        assert(E.final().is_enabled(next));
                        S.push_back(next);
                        break;
                    } else {
                        DEBUGNL(tabs() << "\t= none");
                    }
                }
            }
            DEBUGNL(tabs() << "= " << S);
            return S;
        }
        
        /**
         @brief Wrapper of mReduction.close.
         */
        void close(const std::string& statistics) const
        {
            mReduction.close(statistics);
        }
        
        // OUTPUT
        
        void dump_state(std::ostream& os, const unsigned int i) const
        {
            os << mState[i] << " ";
            mReduction.dump_state(os, i);
        }
        
        // DEBUGGING
        
        static std::string full_name()
        {
            std::string full_name = name();
            full_name += "<";
            full_name += Reduction::full_name();
            full_name += ">";
            return full_name;
        }
        
    private:
        
        // DATA MEMBERS
        
        std::vector<DFSState> mState;
        Reduction mReduction;
        
        // HELPER FUNCTIONS
        
        void pop_back(execution& E, scheduler::schedule_t& S)
        {
            E.pop_last();
            mState.pop_back();
            mReduction.pop_back();
            S.pop_back();
        }
        
        // DEBUGGING
        
        static std::string tabs()
        {
            return "\t";
        }
        
        static std::string outputname()
        {
            std::string outputname = tabs();
            outputname += text_color(name(), utils::io::Color::BLUE);
            return outputname;
        }
    }; // end class template DFS<Reduction>
} // end namespace exploration

#endif
