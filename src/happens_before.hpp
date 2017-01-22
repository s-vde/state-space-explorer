#pragma once

// EXPLORATION
#include "vector_clock.hpp"

// PROGRAM_MODEL
#include "execution.hpp"
#include "instruction_io.hpp"
#include "state.hpp"

// UTILS
#include "container_output.hpp"
#include "debug.hpp"

// DATASTRUCTURES
#include "fixed_size_vector.hpp"

//--------------------------------------------------------------------------------------90
/// @file happens_before.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace exploration
{
	using namespace program_model;
	
	class HappensBeforeBase
	{
	public:
      
      using frontier_t = datastructures::fixed_size_vector<VectorClock>;
		
		// TYPES

      using execution_t = Execution;
      using transition_t = execution_t::transition_t;
		using relation = std::vector<VectorClock>;
		using index_t = typename execution_t::index_t;
	
		// CTOR
		
		explicit HappensBeforeBase(const execution_t& E)
		: mE(E)
		, mHB({ VectorClock(mE.nr_threads()) })
		, mFrontier(mE.nr_threads(), VectorClock(mE.nr_threads()))
		, mIndex(0) { }
		
		//
		
		/**
		 @brief Returns HB(mE[i1], mE[i2]).
		 */
		bool happens_before(const index_t i1, const index_t i2) const;
		
		/**
		 @brief Removes incoming edges from clock that are also incoming
		 edges in mHB[i1].
		 */
		void transitive_reduction(const index_t i1, VectorClock& clock2) const;
		
		/**
		 @brief Removes tid-thread-transitive edges from the given clock.
		 */
		void thread_transitive_reduction(
			const index_t i, const Thread::tid_t& tid, VectorClock& clock) const;
		
		/**
		 @brief Let E' := pre(E,i). The thread transitive dependence relation
		 ->_{E', tid} desribed in @cite flanagan-popl-95 is defined such that
		 j ->_{E'} tid holds if either
		 - E[j].tid = tid; or
		 - there exists a k in dom(E') . j ->_{E'} k && E[k].tid = tid.
		 @details The function's parameter ifrom restricts the relation to
		 vertices with index greater than ifrom.
		 */
		VectorClock::indices_t thread_transitive_relation(
			const index_t i, const index_t ifrom, const Thread::tid_t tid) const;
		
		/**
		 @brief Returns <code>{ i1 < j < i2 | !hb(mE[i1],mE[j]) }</code>.
		 */
		VectorClock::values_t incomparable_after(const index_t i1, const index_t i2) const;
		
		/**
		 @brief The Front(subseq) contains the indices i of Transitions in subseq
		 such that there is no j such that HB(subseq[j], subseq[i]).
		 */
		VectorClock::values_t front(const VectorClock::indices_t& subseq) const;
		
		/**
		 @brief Pops the last element of mHB and sets mIndex to 0 (i.e.
		 mFrontier is no longer valid, because mFrontier is not reset).
		 */
		void pop_back();
		
		/**
		 @brief Performs a reset of mFrontier and sets mIndex to 0.
		 */
		void reset();
		
		/**
		 @brief Restores mFrontier to be valid for mE[i] using mHB.
		 */
		void restore(const index_t i);
		
		Tids tids(const VectorClock::values_t& indices) const;
		
	protected:
		
		// DATA MEMBERS
		
		/// @brief Reference to execution_t object to which this HappensBefore
		/// relation is attached.
		const execution_t& mE;
		
		/// @brief The actual HappensBefore relation.
		relation mHB;
		
		/// @brief Caching the edges of the last Transition by each Thread in pre+(mE, mIndex).
		frontier_t mFrontier;
		
		/// @brief Index in mE with which mFrontier is corresponding.
		unsigned int mIndex;
		
		// OPERATORS

		/**
		 @details Like std::vector[], this subscript operator does not throw
		 if mHB.size() > i and yields undefined behaviour otherwise.
		 */
		const VectorClock& operator[](const index_t i) const;

		// HELPER FUNCTIONS

		void update_frontier(const transition_t& t, const VectorClock& clock);
		
		VectorClock::index_t max_dependent(
			const index_t i, const Instruction& instr, const bool ttr, VectorClock C) const;
		
		/**
		 @brief Returns <code>{ 0 < j < index | E[j] <: E[i] }</code>,
		 where <code>E[j] <: E[i]</code> iff <code>hb(E[j],E[i]) &&
		 !exists k . hb(E[j],E[k]) && hb(E[k],E[i])</code>.
		 @complexity O(n^2) with n = |clock|.
		 */
		VectorClock::indices_t covering(
			const index_t i, const Instruction& instr, VectorClock C) const;
		
		// PRE/POST CONDITIONS
		
		/*bool invariant1() const
		 {
		 return mIndex <= mHB.size()-1;
		 }*/
		
		/**
		 @brief This HappensBefore is restored from a reset iff mIndex == mHB.size()-1.
		 */
		bool restored() const;
		
		bool not_restored() const;
		
		bool frontier_valid_for(const index_t i) const;
		
		/**
		 @brief Returns true iff the happens-before relation is already defined
		 on the prefix pre+(mE,i).
		 */
		bool defined_on_prefix(const index_t i) const;
		
		// DEBUGGING
		
		static std::string name();
		static std::string tabs();
		static std::string outputname();
		
	private:
		
		bool happens_before(const index_t i1, const VectorClock& clock2) const;
		
		/**
		 @brief Returns the VectorClock corresponding to the previous
		 Transition by tid in pre(E,i).tid.
		 @note The case where mE[mIndex].instr().tid == tid and where it is not
		 yield different types of clocks, i.e. with different values set for tid.
		 */
		const VectorClock& previous_by(const Thread::tid_t& tid) const;
		
	}; // end class HappensBeforeBase
	
    template<typename Dependence>
	class HappensBefore : public HappensBeforeBase
    {
    public:
	
        // CTOR
		
		explicit HappensBefore(const execution_t& E)
		: HappensBeforeBase(E) { }
		
        //

        /**
         @brief Adds a new VectorClock corresponding to the happens-before
         edges of Transition mE[i] to mHB.
         */
        void update(const index_t i)
        {
            /// @pre defined_on_prefix(i-1) && frontier_valid_for(i-1)
            assert(defined_on_prefix(i-1) && frontier_valid_for(i-1));
            DEBUGFNL(outputname(), "update", "[" << i << "]", "");
            mHB.push_back(create_clock(i, mE[i].instr()));
            update_frontier(mE[i], mHB.back());
            /// @post defined_on_prefix(i) && frontier_valid_for(i)
            assert(defined_on_prefix(i) && frontier_valid_for(i));
        }

        /**
         @brief Returns the index of the most recent Transition in 
         pre(mE,i) that is dependent with instr (and satisfies
         the given other conditions). Returns 0 iff there is no such 
         Transition.
        */
		// #todo coenabledness
        VectorClock::index_t max_dependent(
			const index_t i,
			const Instruction& instr,
			const bool ttr=true) const
        {
			/// @pre frontier_valid_for(i)
			assert(frontier_valid_for(i));
			return HappensBeforeBase::max_dependent(i, instr, ttr, clock(i, instr));
        }
        
        /**
         @brief Returns for each Thread::tid_t tid' with tid' != instr.tid
         the most recent Transition of tid' in pre(mE,i) that is 
         dependent with instr, if it exists.
         */
		// #todo coenabledness
        VectorClock::indices_t max_dependent_per_thread(
            const index_t i,
            const Instruction& instr,
            const bool use_thread_transitive_reduction=true) const
        {
			/// @pre frontier_valid_for(i)
			assert(frontier_valid_for(i));
            DEBUGFNL("\t" << outputname(), "max_dependent_per_thread", "[" << i << "], " << instr, "");
            VectorClock::indices_t MaxDep{};
            VectorClock C = clock(i, instr);
            if (use_thread_transitive_reduction) {
                thread_transitive_reduction(i, instr.tid(), C);
            }
            C[instr.tid()] = 0; // exclude instr.tid-dependencies
            VectorClock::index_t j;
            while (j = max_element(C), j > 0) {
                const Instruction& instr_j = mE[j].instr();
                if (Dependence::dependent(instr_j, instr)) {
                    MaxDep.insert(j);
                    C[instr_j.tid()] = 0;
                } else {
                    // check previous Transition by instr_i.tid
                    // #todo show thread-transitive-red still holds.
                    C[instr_j.tid()] = mHB[j][instr_j.tid()];
                }
            }
            DEBUG(" = " << MaxDep);
            return MaxDep;
        }
        
        /**
         @brief Returns <code>{ 0 < j < index | E[j] <: E[i] }</code>,
         where <code>E[j] <: E[i]</code> iff <code>hb(E[j],E[i]) && 
         !exists k . hb(E[j],E[k]) && hb(E[k],E[i])</code>.
         @complexity O(n^2) with n = |clock|.
         */
        VectorClock::indices_t covering(const index_t i, const Instruction& instr) const
        {
			return HappensBeforeBase::covering(i, instr, clock(i, instr));
        }
        
    private:

        /**
         @brief Returns the happens-before edges for instr in pre(mE,i).instr.
         @note Yields undefined behaviour if instr.tid == mE[i].instr.tid
         but !defined_on_prefix(i).
         */
        VectorClock clock(const index_t i, const Instruction& instr) const
        {
            return
                instr.tid() == mE[i].instr().tid()
                ? (*this)[i]
                : create_clock(i, instr);
        }

        /**
         @brief Creates the HappensBefore edges corresponding to Instruction instr 
		 after E' = pre(mE,i).
         
         @details The function iterates backwards through E' to find for every 
		 Thread tid such that tid != instr.tid() the maximal j such that 
		 happens_before(E'[j], instr). To avoid traversing the whole sequence E', 
		 it maintains a minimum index min, which is initialized by 
		 frontier[instr.tid()].min --- updated every time the clock is updated.
         
         @note We define a happens-before relation to be irreflexive.
         Therefore, the VectorClock corresponding to Transition t
         refers to the previous Transition by t.instr.get_tid() in clock[t.instr.tid].
         However, frontier[t.instr.tid][t.instr.tid] = index.
         */
        VectorClock create_clock(const index_t i, const Instruction& instr) const
        {
            /// @pre (frontier_valid_for(i-1) && instr.tid() == mE[i]) ||
            ///      (frontier_valid_for(i) && instr.tid() != mE[i])
            assert(
                (frontier_valid_for(i-1) && instr.tid() == mE[i].instr().tid()) ||
                (frontier_valid_for(i) && instr.tid() != mE[i].instr().tid())
            );
            VectorClock C = mFrontier[instr.tid()];
            DEBUGF("\t" << outputname(), "create_clock", "[" << i << "] " << instr, " = MAX( " << C);
            int min = min_element(C);
            for (int j = i-1; j > min; --j) {
                const Instruction& instr_j = mE[j].instr();
                // j -!>_pre(E,i) instr.tid
                if (j > C[instr_j.tid()] && Dependence::dependent(instr_j, instr)) {
                    C.max(mHB[j]);
                    C[instr_j.tid()] = j;
					min = min_element(C);
                    DEBUG(", " << mHB[j] << "[" << instr_j.tid() << ":=" << j << "]");
                }
            }
            /// @note clock[instr.tid] < i.
            DEBUGNL(" ) = " << C);
            return C;
        }
		
    }; // end class template HappensBefore<Dependence>
} // end namespace exploration
