
#ifndef BOUND_PERSISTENT_SET_HPP_INCLUDED
#define BOUND_PERSISTENT_SET_HPP_INCLUDED

#include "bound.hpp"
#include "persistent_set.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file bound_persistent_set.hpp
 @brief Definition of classes BoundPersistentState and BoundPersistentBase
 and class template and BoundPersistent<BoundFunction>.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace exploration
{
    class BoundPersistentState : public BoundState
    {
    public:
        
        // CTOR
        
        explicit BoundPersistentState(const int boundvalue=0);
        
        //
        
        void add_to_pending(const Thread::tid_t& tid);
        void add_to_pending(const Tids& tids);
        const Tids& pending() const;
        
        void set_bound_exceeded();
        bool bound_exceeded() const;
        
        std::string to_string() const;
        
    private:
        
        // DATA MEMBERS
        
        Tids mPending;
        
        /**
         @brief Whether the bound was exceeded in the exploration subtree
         rooted by this State.
         */
        bool mBoundExceeded;
        
    }; // end class BoundPersistentState
    
    std::ostream& operator<<(std::ostream&, const BoundPersistentState&);
	
	/**
	 Optimizations to further reduce the state-space described in
	 @cite coons-thesis:
	 */
	class BoundPersistentOptimizations
	{
	public:
		
		enum sleep_t
		{
			NEVER,
			
			/**
			 @brief When backtracking Transition t, place t.instr.tid in the
			 sleepset at t.pre iff the bound was not exceeded in the subtree
			 rooted by t.
			 */
			CONSERVATIVE
			
			// #todo, AGGRESSIVE
		};
		
		explicit BoundPersistentOptimizations(
			const bool transred=true,
			const bool alternative=true,
			const bool bound_opt=true,
			const sleep_t sleepsets=CONSERVATIVE
		);
		
		bool TRANSITIVE_REDUCTION() const;
		bool ALTERNATIVE_THREAD() const;
		bool BOUND_OPT() const;
		sleep_t SLEEPSETS() const;
				
	private:
		
		/**
		 @brief Add backtrack points only for dependencies that are in the
		 thread transitive reduction of the HappensBefore relation on the
		 Execution (i.e. the minimal graph representing the same partial
		 order).
		 */
		bool mTRANSITIVE_REDUCTION;
		
		/**
		 @brief If a Thread is disabled at a backtracking point, instead of
		 adding *all* enabled Threads check whether there is a *single*
		 alternative Transition that can be added to maintain coverage.
		 */
		bool mALTERNATIVE_THREAD;
		
		// #todo bool mRELEASE_OPT;
		
		/**
		 @brief Allows BoundPersistents to behave exactly like
		 Persistents with a sufficiently high bound.
		 */
		bool mBOUND_OPT;
		
		/**
		 @details Search can place a tid in the sleepset at a visited node s
		 because it explored the *entire* state-space reachable from
		 s.next(tid). Enum sleep_t enumerates the restrictions of sleepsets
		 that can be used with BoundPersistents.
		 */
		sleep_t mSLEEPSETS;
		
	}; // end class BoundPersistentOptimizations

	class BoundPersistentBase
	{
	public:
		
		// TYPES
		
		using execution = Execution;
      using transition = typename Execution::transition_t;
		
		//
		
		/**
		 @brief Returns true.
		 */
		bool check_valid(const bool contains_locks) const
		{
			return true;
		}
		
		/**
		 @cite coons-oopsla-13.
		 @code{.cpp}
		 BacktrackPoints BoundPersistent::backtrack_points(E, i)
		 {
		 	Points = {};
		 	State s = E[i].pre();
		 	for all(p in Tids | s.has_next(p)) {
		 		for all(q in Tids) {
		 			Dep = { j in dom(E) | E_j.tid == q && dependent(E[j], s.next(p)) };
					if (!Dep.empty()) { Points.insert((p, max(Dep))); }
				 }
		 	}
		 	return Points;
		 }
		 @endcode
		 @see HappensBefore::max_dependent_per_thread
		 */
		template<typename Dependence>
		BacktrackPoints backtrack_points(
			const execution& E,
			const unsigned int index,
			const HappensBefore<Dependence>& HB) const
		{
			DEBUGF(outputname(), "backtrack_points", to_short_string(E[index]), "\n");
			BacktrackPoints Points{};
			const State& s = E[index].pre();
			std::for_each(
				s.next_cbegin(), s.next_cend(),
				[this, &index, &HB, &Points] (const auto& next) {
					VectorClock::indices_t points =
					HB.max_dependent_per_thread(
						index,
						next.second.instr,
						mOpt.TRANSITIVE_REDUCTION()
					);
					/// @note @cite coons-thesis checks for transitive reduction before
					/// adding backtrack points, but it seems like applying transitive
					/// reduction here yields equivalent behaviour.
					for (const auto& index : points) {
						Points.push_back({ next.first, index });
						DEBUG(tabs << "\tPoints.add(" << Points.back() << ")\n");
					}
				}
			);
			return Points;
		}
		
		/**
		 @brief Adds mState.back().pending() to pool.
		 */
		void add_to_pool(Tids& pool) const;
		
		/**
		 @brief Propagates t.post.mBoundExceeded to t.pre.mBoundExceeded iff
		 t.post.mBoundExceeded == true and updates the sleepset at t.pre
		 depending on the mOpt.SLEEPSETS() setting.
		 */
		void update_after_exploration(const transition& t, SufficientSet& t_pre);
		
		void pop_back();
		
	protected:
		
		BoundPersistentBase();
		
		// DATA MEMBERS
		
		std::vector<BoundPersistentState> mState;
		BoundPersistentOptimizations mOpt;
		
		//
		
		/**
		 @code{.cpp}
		 Tids alternatives(E, index, point)
		 {
		 E' := pre(E, index);
		 return { tid' in Tids |
		 tid' = tid ||
		 tid' = E'[j].tid && point.index < j < index && j ->_E' tid
		 }
		 }
		 @endcode
		 */
		template<typename Dependence>
		Tids alternatives(
			const execution& E,
			const unsigned int index,
			const SufficientSet& s,
			const HappensBefore<Dependence>& HB,
			const backtrack_point& point) const
		{
			return Persistent::alternatives(E, index, s, HB, point, mOpt.SLEEPSETS() != mOpt.sleep_t::NEVER);
		}
		
		bool adding_condition(const State& s, const SufficientSet& suf, const Thread::tid_t& tid) const;
		
		Tids adding_tids(const State& s, const SufficientSet& suf) const;
		
		static const std::string name;
		static const std::string tabs;
		static std::string outputname();
		
	}; // end class BoundPersistentBase
	
    /**
     @brief Implements bound persistent sets as defined in @cite coons-oopsla-13.
	 These allow the combination of partial order reduction with bounded search
	 while providing bounded-completeness with respect to the BoundFunction and
     a bound k.
	 
     @details k-BoundFunction-bounded-completeness of an exploration of
     Program P's state-space means that for every Execution E of P with
     BoundFunction.value(E) <= k the exploration has seen an Execution E'
     that is equivalent to E with respect to a suitable HappensBefore 
     relation.
     
     Optimizations to further reduce the state-space described in 
     @cite coons-thesis are implemented, and can be turned on/off using
     an object of class optimizations.
     */
	// #todo The current implementation is designed for use with Preemptions
	// as argument to template parameter BoundFunction. The Preemptions
	// specific code should be put in a separate template instantiation
	// class.
	template<typename BoundFunction>
	class BoundPersistent : public BoundPersistentBase
    {
    public:
		
		// TYPES
		
		using bound_t = typename BoundFunction::value_t;
        
        // CTOR
        
		explicit BoundPersistent(const typename BoundFunction::value_t bound)
        : BoundPersistentBase()
		, mBound(bound) { }
		
		//
        
        void update_state(const execution& E, const transition& t)
        {
            const auto tid = boost::apply_visitor(program_model::get_tid(), t.instr());
            mState.emplace_back(BoundFunction::value(E, mState, t.index()-1, tid));
            DEBUGF(outputname(), "update_state", t.instr(), to_string_post_state(t) << ".boundvalue = " << mState.back().boundvalue() << "\n");
        }
        
        /**
         Implementation from @cite coons-oopsla-13 with optimization
         from @cite coons-thesis.
         @code{.cpp}
         BoundPersistent::add_backtrack_point(E, index, point, conservative=false) 
		 {
            State s = E[point.index].pre();
            if (point.tid in s.enabled()) {
                if (!conservative) then s.backtrack.add(point.tid);
                else s.pending.add(point.tid);
            } else {
                if (!conservative) then s.backtrack() = s.enabled();
                else s.pending() = s.enabled();
            }
            if (!conservative) {
                ContextSwitches := { j in dom(E) | j < point.index &&
                    (j == 1 || E[j-1].instr.tid != E[j].instr.tid };
                if (!ContextSwitches.empty()) {
                    AddBacktrackPoint(E, index, (j, point.tid), true);
                }
            }
         }
         @endcode
         @note Currently, point.tid is prioritized to avoid an
         unnecessary call to alternatives. However, other selection
         strategies are feasible.
         @note Current implementation does prioritize point.tid iff it
         is an alternative with *least* boundvalue.
         */
		template<typename Dependence>
        void add_backtrack_point(
            const execution& E,
            const unsigned int index,
            std::vector<SufficientSet>& S,
            const HappensBefore<Dependence>& HB,
            const backtrack_point& point,
            bool conservative=false)
        {
            DEBUGF(outputname(), "add_backtrack_point" << (conservative ? "_conservative" : ""), point, "\n");
            Thread::tid_t alt = point.tid;
            if (mOpt.ALTERNATIVE_THREAD()) {
                auto Alt = alternatives(E, index, S[point.index-1], HB, point);
                if (!Alt.empty()) {
                    alt = *(BoundFunction::min_value(E, point.index-1, Alt, {point.tid}));
                }
            }
            Tids Add{};
            if (adding_condition(E[point.index].pre(), S[point.index-1], alt)) {
                Add = { alt };
            } else {
                /// @invariant !(E[point.index].pre().is_enabled(alt)) -> alt == point.tid
                assert(alt == point.tid);
                Add = adding_tids(E[point.index].pre(), S[point.index-1]);
            }
			DEBUG(
				tabs << to_string_pre(point.index) << "."
				<< ((conservative && mOpt.BOUND_OPT()) ? "pending" : "backtrack")
				<< ".add(" << Add << ")"
			);
            if (!conservative)          { S[point.index-1].add_to_backtrack(Add);       }
            else if (mOpt.BOUND_OPT())  { mState[point.index-1].add_to_pending(Add);    }
            else                        { S[point.index-1].add_to_backtrack(Add);       }
		
            if (!conservative) {
                const auto conservative_index = BoundFunction::last_context_switch_before(E, point.index);
                if (conservative_index < point.index) {
                    add_backtrack_point(
						E, index, S, HB,
						{ point.tid, static_cast<int>(conservative_index) },
						true
					);
                }
            }
        }
        
        /**
         @brief Returns true iff BoundFunction::value(E.next(tid)) <= bound.
         If not, E.final.bound_exceeded is set to true and 
         E.final.sleepset is cleared.
         */
		bool condition(const execution& E, SufficientSet& s, const Thread::tid_t& tid)
        {
            if (BoundFunction::value(E, mState, E.size(), tid) <= mBound) {
                DEBUGF(outputname(), "condition", tid, " = true\n");
                return true;
            } else {
                mState.back().set_bound_exceeded();
                DEBUGF(outputname(), "condition", tid, " = false\n");
                return false;
            }
        }
        
    private:
        
        // DATA MEMBERS
		
		typename BoundFunction::value_t mBound;
		
    }; // end class template BoundPersistent<BoundFunction>
} // end namespace exploration

#endif
