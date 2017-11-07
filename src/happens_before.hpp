#pragma once

#include "vector_clock.hpp"

#include "execution.hpp"
#include "state.hpp"
#include "visible_instruction_io.hpp"

#include "color_output.hpp"
#include "container_output.hpp"
#include "debug.hpp"

#include "fixed_size_vector.hpp"

//--------------------------------------------------------------------------------------------------
/// @file happens_before.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//--------------------------------------------------------------------------------------------------


namespace exploration {

namespace detail {

using execution_t = program_model::Execution;
using transition_t = execution_t::transition_t;
using instruction_t = transition_t::instruction_t;

/// @brief Creates the HappensBefore edges to the given instruction after pre(execution,index),
/// defined as max{clock(j) | j in [1..index-1) and dependent(execution[j], instruction)}.
/// @note To see that it is not necessary to start from frntier(index, instruction.tid):
/// let last_seen_tid = frontier(index, instruction.tid)[tid].
/// for every j in dom(execution) | j > last_seen_tid it holds that
/// j > frontier(index, instruction.tid)[execution[j].tid] and hence
/// j -/>_pre(execution,index) instruction.tid
/// @note We define a happens-before relation to be irreflexive. Therefore, the VectorClock
/// corresponding to the given instruction after pre(execution,index) refers to the previous
/// Transition by instruction.tid() in clock[instruction.tid]. However,
/// frontier[t.instr.tid][t.instr.tid] = index.

template <typename Dependence>
VectorClock create_clock(const execution_t& execution,
                         const std::vector<VectorClock> happens_before_relation,
                         const execution_t::index_t index, const instruction_t& instruction)
{
   assert(happens_before_relation.size() >= index - 1);

   VectorClock clock(execution.nr_threads());
   DEBUGF(text_color("HappensBefore", utils::io::Color::YELLOW), "create_clock",
          "pre(execution, " << index << ")." << boost::apply_visitor(program_model::instruction_to_short_string(), instruction), "\n= MAX (\n");
   DEBUG("\t" << clock << "\n");

   int min = min_element(clock);
   const auto tid = boost::apply_visitor(program_model::get_tid(), instruction);

   for (int j = index - 1; j > min; --j)
   {
      const instruction_t& instruction_j = execution[j].instr();
      const auto tid_j = boost::apply_visitor(program_model::get_tid(), instruction_j);

      // j -!>_pre(execution,index) instruction.tid
      if (j > clock[tid_j] && Dependence::dependent(instruction_j, instruction))
      {
         clock.max(happens_before_relation[j]);
         clock[tid_j] = j;
         min = min_element(clock);
         DEBUG("\t" << happens_before_relation[j] << "[" << tid_j << ":=" << j << "]\n");
      }
   }
   DEBUG(") = " << clock << "\n");
   return clock;
}

} // namespace detail

//--------------------------------------------------------------------------------------------------


class HappensBeforeBase
{
public:
   using frontier_t = datastructures::fixed_size_vector<VectorClock>;

   using execution_t = program_model::Execution;
   using transition_t = execution_t::transition_t;
   using instruction_t = transition_t::instruction_t;
   using relation = std::vector<VectorClock>;
   using index_t = typename execution_t::index_t;

   explicit HappensBeforeBase(const execution_t& E)
   : mE(E)
   , mHB({VectorClock(mE.nr_threads())})
   , mFrontier(mE.nr_threads(), VectorClock(mE.nr_threads()))
   , mIndex(0)
   {
   }

   /// @brief Returns HB(mE[i1], mE[i2]).

   bool happens_before(const index_t i1, const index_t i2) const;

   /// @brief Removes incoming edges from clock that are also incoming edges in mHB[i1].

   void transitive_reduction(const index_t i1, VectorClock& clock2) const;

   /// @brief Removes tid-thread-transitive edges from the given clock.

   void thread_transitive_reduction(const index_t i, const program_model::Thread::tid_t& tid,
                                    VectorClock& clock) const;

   /// @brief Let E' := pre(E,i). The thread transitive dependence relation ->_{E', tid} desribed in
   /// @cite flanagan-popl-95 is defined such that j ->_{E'} tid holds if either
   /// - E[j].tid = tid; or
   /// - there exists a k in dom(E') . j ->_{E'} k && E[k].tid = tid.
   /// @details The function's parameter ifrom restricts the relation to vertices with index greater
   /// than ifrom.

   VectorClock::indices_t thread_transitive_relation(const index_t i, const index_t ifrom,
                                                     const program_model::Thread::tid_t tid) const;

   /// @brief Returns <code>{ i1 < j < i2 | !hb(mE[i1],mE[j]) }</code>.

   VectorClock::values_t incomparable_after(const index_t i1, const index_t i2) const;

   /// @brief The Front(subseq) contains the indices i of Transitions in subseq such that there is
   /// no j such that HB(subseq[j], subseq[i]).

   VectorClock::values_t front(const VectorClock::indices_t& subseq) const;

   /// @brief Pops the last element of mHB and sets mIndex to 0 (i.e. mFrontier is no longer valid,
   /// because mFrontier is not reset).

   void pop_back();

   /// @brief Reset this HappensBeforeBase to the potentially updated underlying execution mE.

   void reset();

   /// @brief Restores mFrontier to be valid for mE[i] using mHB.

   void restore(const index_t i);

   program_model::Tids tids(const VectorClock::values_t& indices) const;

protected:
   /// @brief Reference to execution_t object to which this HappensBefore
   /// relation is attached.
   const execution_t& mE;

   /// @brief The actual HappensBefore relation.
   relation mHB;

   /// @brief Caching the edges of the last Transition by each program_model::Thread in pre+(mE,
   /// mIndex).
   frontier_t mFrontier;

   /// @brief Index in mE with which mFrontier is corresponding.
   unsigned int mIndex;

   /**
    @details Like std::vector[], this subscript operator does not throw
    if mHB.size() > i and yields undefined behaviour otherwise.
    */
   const VectorClock& operator[](const index_t i) const;

   void update_frontier(const transition_t& t, const VectorClock& clock);

   /**
    @brief Returns <code>{ 0 < j < index | E[j] <: E[i] }</code>,
    where <code>E[j] <: E[i]</code> iff <code>hb(E[j],E[i]) &&
    !exists k . hb(E[j],E[k]) && hb(E[k],E[i])</code>.
    @complexity O(n^2) with n = |clock|.
    */
   VectorClock::indices_t covering(const index_t i, const instruction_t& instr,
                                   VectorClock C) const;

   /// @brief This HappensBefore is restored from a reset iff mIndex == mHB.size()-1.

   bool restored() const;

   bool not_restored() const;

   bool frontier_valid_for(const index_t i) const;

   /// @brief Returns true iff the happens-before relation is already defined on the prefix
   /// pre+(mE,i).

   bool defined_on_prefix(const index_t i) const;

   static std::string name();
   static std::string tabs();
   static std::string outputname();

private:
   bool happens_before(const index_t i1, const VectorClock& clock2) const;

   /// @brief Returns the VectorClock corresponding to the previous Transition by tid in
   /// pre(E,i).tid.
   /// @note The case where mE[mIndex].instr().tid == tid and where it is not yield different types
   /// of clocks, i.e. with different values set for tid.

   const VectorClock& previous_by(const program_model::Thread::tid_t& tid) const;

}; // end class HappensBeforeBase


template <typename Dependence>
class HappensBefore : public HappensBeforeBase
{
public:
   explicit HappensBefore(const execution_t& E)
   : HappensBeforeBase(E)
   {
   }

   /// @brief Adds a new VectorClock corresponding to the happens-before edges of Transition mE[i]
   /// to mHB.

   void update(const index_t i);

   /// @brief Returns the index of the most recent Transition in pre(mE,index) that is dependent
   /// with the given instruction (and satisfies the given other conditions). Returns 0 iff there
   /// is no such Transition.

   VectorClock::index_t max_dependent(const index_t index, const instruction_t& instruction,
                                      const bool apply_thread_transitive_reduction = true,
                                      const bool apply_coenabled = false) const;

   /// @brief Returns for each program_model::Thread::tid_t tid' with tid' != instr.tid the most
   /// recent Transition of tid' in pre(mE,i) that is dependent with instr, if it exists.

   // #todo coenabledness
   VectorClock::indices_t max_dependent_per_thread(
      const index_t i, const instruction_t& instr,
      const bool use_thread_transitive_reduction = true) const;

   /// @brief Returns <code>{ 0 < j < index | E[j] <: E[i] }</code>, where <code>E[j] <: E[i]</code>
   /// iff <code>hb(E[j],E[i]) && !exists k . hb(E[j],E[k]) && hb(E[k],E[i])</code>.
   /// @complexity O(n^2) with n = |clock|.

   VectorClock::indices_t covering(const index_t i, const instruction_t& instr) const;

private:
   /// @brief Returns the happens-before edges for instr in pre(mE,i).instr.
   /// @note Yields undefined behaviour if instr.tid == mE[i].instr.tid but !defined_on_prefix(i).

   VectorClock clock(const index_t i, const instruction_t& instr) const;

}; // end class template HappensBefore<Dependence>

//--------------------------------------------------------------------------------------------------

template <typename Dependence>
void HappensBefore<Dependence>::update(const index_t i)
{
   /// @pre defined_on_prefix(i-1) && frontier_valid_for(i-1)
   assert(defined_on_prefix(i - 1) && frontier_valid_for(i - 1));
   DEBUGF(outputname(), "update", "[" << i << "]", "\n");
   mHB.push_back(detail::create_clock<Dependence>(mE, mHB, i, mE[i].instr()));
   update_frontier(mE[i], mHB.back());
   /// @post defined_on_prefix(i) && frontier_valid_for(i)
   assert(defined_on_prefix(i) && frontier_valid_for(i));
}

//--------------------------------------------------------------------------------------------------

template <typename Dependence>
VectorClock::index_t HappensBefore<Dependence>::max_dependent(
   const index_t index, const instruction_t& instruction,
   const bool apply_thread_transitive_reduction, const bool apply_coenabled) const
{
   /// @pre frontier_valid_for(index)
   assert(frontier_valid_for(index));

   VectorClock C = clock(index, instruction);

   DEBUGF("--------------------\nHappensBefore", "max_dependent",
          "[" << index << "], " << boost::apply_visitor(
             program_model::instruction_to_short_string(), instruction) << (apply_coenabled ? ", coenabled" : ""), "\n");

   const auto tid = boost::apply_visitor(program_model::get_tid(), instruction);

   if (apply_thread_transitive_reduction)
   {
      thread_transitive_reduction(index, tid, C);
   }
   // exclude instr.tid-dependencies
   C[tid] = 0;

   auto max_it = std::max_element(C.cbegin(), C.cend());

   //
   if (apply_coenabled)
   {
      while (*max_it > 0 && (!Dependence::dependent(mE[*max_it].instr(), instruction) ||
                             !Dependence::coenabled(mE[*max_it].instr(), instruction)))
      {
         DEBUG(boost::apply_visitor(program_model::instruction_to_short_string(), mE[*max_it].instr()));
         program_model::Thread::tid_t max_tid = std::distance(C.cbegin(), max_it);
         C[max_tid] = mHB[*max_it][max_tid];
         max_it = std::max_element(C.cbegin(), C.cend());
      }
   }

   DEBUG(" = " << *max_it);
   return *max_it;
}

//--------------------------------------------------------------------------------------------------

template <typename Dependence>
VectorClock::indices_t HappensBefore<Dependence>::max_dependent_per_thread(
   const index_t i, const instruction_t& instr, const bool use_thread_transitive_reduction) const
{
   /// @pre frontier_valid_for(i)
   assert(frontier_valid_for(i));
   DEBUGF("\t" << outputname(), "max_dependent_per_thread", "[" << i << "], " << instr, "\n");
   VectorClock::indices_t MaxDep{};
   VectorClock C = clock(i, instr);
   const auto tid = boost::apply_visitor(program_model::get_tid(), instr);
   if (use_thread_transitive_reduction)
   {
      thread_transitive_reduction(i, tid, C);
   }
   C[tid] = 0; // exclude instr.tid-dependencies
   VectorClock::index_t j;
   while (j = max_element(C), j > 0)
   {
      const instruction_t& instr_j = mE[j].instr();
      const auto tid_j = boost::apply_visitor(program_model::get_tid(), instr_j);
      if (Dependence::dependent(instr_j, instr))
      {
         MaxDep.insert(j);
         C[tid_j] = 0;
      }
      else
      {
         // check previous Transition by instr_i.tid
         // #todo show thread-transitive-red still holds.
         C[tid_j] = mHB[j][tid_j];
      }
   }
   DEBUG(" = " << MaxDep);
   return MaxDep;
}

//--------------------------------------------------------------------------------------------------

template <typename Dependence>
VectorClock::indices_t HappensBefore<Dependence>::covering(const index_t i,
                                                           const instruction_t& instr) const
{
   return HappensBeforeBase::covering(i, instr, clock(i, instr));
}

//--------------------------------------------------------------------------------------------------

template <typename Dependence>
VectorClock HappensBefore<Dependence>::clock(const index_t i, const instruction_t& instr) const
{
   const auto tid = boost::apply_visitor(program_model::get_tid(), instr);
   const auto tid_i = boost::apply_visitor(program_model::get_tid(), mE[i].instr());
   return tid == tid_i ? (*this)[i] : detail::create_clock<Dependence>(mE, mHB, i, instr);
}

//--------------------------------------------------------------------------------------------------

} // end namespace exploration
