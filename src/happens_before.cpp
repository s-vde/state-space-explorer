
#include "happens_before.hpp"
#include "algo.hpp"
#include "color_output.hpp"


namespace exploration {

//--------------------------------------------------------------------------------------------------

bool HappensBeforeBase::happens_before(const index_t i1, const index_t i2) const
{
   /// @pre defined_on_prefix(max(i1,i2))
   assert(defined_on_prefix(std::max(i1, i2)));
   return happens_before(i1, (*this)[i2]);
}

//--------------------------------------------------------------------------------------------------

void HappensBeforeBase::transitive_reduction(const index_t i1, VectorClock& clock2) const
{
   /// @pre happens_before(i1, clock2)
   assert(happens_before(i1, clock2));
   clock2.filter_values_greater_than((*this)[i1]);
}

//--------------------------------------------------------------------------------------------------

void HappensBeforeBase::thread_transitive_reduction(const index_t i,
                                                    const program_model::Thread::tid_t& tid,
                                                    VectorClock& clock) const
{
   /// @pre frontier_valid_for(i)
   assert(frontier_valid_for(i));
   clock.filter_values_greater_than(previous_by(tid));
   clock[tid] = 0;
}

//--------------------------------------------------------------------------------------------------

VectorClock::indices_t HappensBeforeBase::thread_transitive_relation(
   const index_t i, const index_t ifrom, const program_model::Thread::tid_t tid) const
{
   /// @pre frontier_valid_for(i)
   assert(frontier_valid_for(i));
   return indices_such_that(previous_by(tid),
                            [&ifrom](const auto& value) { return value > ifrom; });
}

//--------------------------------------------------------------------------------------------------

VectorClock::values_t HappensBeforeBase::incomparable_after(const index_t i1,
                                                            const index_t i2) const
{
   VectorClock::values_t Incomparable{};
   for (int j = i1 + 1; j < i2; ++j)
   {
      if (!happens_before(i1, j))
      {
         Incomparable.insert(j);
      }
   }
   DEBUGFNL("\t" << outputname(), "incomparable_after", "[" << i1 << "]", " = " << Incomparable);
   return Incomparable;
}

//--------------------------------------------------------------------------------------------------

VectorClock::values_t HappensBeforeBase::front(const VectorClock::indices_t& subseq) const
{
   VectorClock::values_t Front{};
   if (!subseq.empty())
   {
      VectorClock first_seen(mE.nr_threads());
      VectorClock last_seen(mE.nr_threads());
      for (const auto& i : subseq)
      {
         const auto tid = boost::apply_visitor(program_model::get_tid(), mE[i].instr());
         const VectorClock& C = (*this)[i];
         if (first_seen[tid] == 0)
         {
            auto seen_before = utils::algo::find_if_with_index(
               C.begin(), C.end(), [&first_seen, &last_seen](const auto& tid_, const auto& val) {
                  return last_seen[tid_] > 0 && first_seen[tid_] <= val && val <= last_seen[tid_];
               });
            if (seen_before != C.end())
            {
               DEBUGNL(tabs() << "\t\t" << i << " notin Front");
            }
            else
            {
               DEBUGNL(tabs() << "\t\t" << i << " in Front");
               Front.insert(i);
            }
            first_seen[tid] = i;
         }
         else
         {
            DEBUGNL(tabs() << "\t\t" << i << " notin Front : not first of " << tid);
            /// @invariant last_seen[tid] == C[tid]
            /// (i.e. subsequence of Transitions by tid in subseq
            /// does not skip Transitions by tid).
            assert(last_seen[tid] == C[tid]);
         }
         last_seen[tid] = i;
      }
   }
   DEBUGFNL("\t" << outputname(), "front", subseq, " = " << Front);
   return Front;
}

//--------------------------------------------------------------------------------------------------

void HappensBeforeBase::pop_back()
{
   mHB.pop_back();
   mIndex = 0;
}

//--------------------------------------------------------------------------------------------------

void HappensBeforeBase::reset()
{
   std::size_t size = mFrontier.size();
   mFrontier = frontier_t(size, VectorClock(size));
   mIndex = 0;
}

//--------------------------------------------------------------------------------------------------

void HappensBeforeBase::restore(const index_t i)
{
   /// @pre not_restored() && frontier_valid_for(index-1) (impl. defined_on_prefix(i-1)).
   assert(not_restored() && frontier_valid_for(i - 1));
   update_frontier(mE[i], mHB[i]);
   /// @post defined_on_prefix(i) && frontier_valid_for(i) (impl. invariant1)
   assert(defined_on_prefix(i) && frontier_valid_for(i));
}

//--------------------------------------------------------------------------------------------------

program_model::Tids HappensBeforeBase::tids(const VectorClock::values_t& indices) const
{
   program_model::Tids T{};
   std::transform(indices.begin(), indices.end(), std::inserter(T, T.end()), [this](const auto& i) {
      return boost::apply_visitor(program_model::get_tid(), mE[i].instr());
   });
   return T;
}

//--------------------------------------------------------------------------------------------------

const VectorClock& HappensBeforeBase::operator[](const index_t i) const
{
   return mHB[i];
}

//--------------------------------------------------------------------------------------------------

void HappensBeforeBase::update_frontier(const transition_t& t, const VectorClock& clock)
{
   const auto tid = boost::apply_visitor(program_model::get_tid(), t.instr());
   mFrontier[tid] = clock;
   mFrontier[tid][tid] = t.index();
   ++mIndex;
}

//--------------------------------------------------------------------------------------------------

VectorClock::indices_t HappensBeforeBase::covering(const index_t i, const instruction_t& instr,
                                                   VectorClock C) const
{
   const auto tid = boost::apply_visitor(program_model::get_tid(), instr);
   thread_transitive_reduction(i, tid, C);
   C[tid] = 0; // exclude instr.tid-dependencies
   VectorClock::indices_t Covering{};
   VectorClock::value_t j = 0;
   // in every loop-iteration an entry in C is set to 0.
   while (j = max_element(C), j > 0)
   {
      Covering.insert(j);
      transitive_reduction(j, C);
      const auto tid_j = boost::apply_visitor(program_model::get_tid(), mE[j].instr());
      C[tid_j] = 0;
   }
   DEBUGFNL("\t" << outputname(), "covering", "[" << i << "], " << instr, " = " << Covering);
   return Covering;
}

//--------------------------------------------------------------------------------------------------

bool HappensBeforeBase::restored() const
{
   return mIndex == mHB.size() - 1;
}

//--------------------------------------------------------------------------------------------------

bool HappensBeforeBase::not_restored() const
{
   return mIndex < mHB.size() - 1;
}

//--------------------------------------------------------------------------------------------------

bool HappensBeforeBase::frontier_valid_for(const index_t i) const
{
   return mIndex == i;
}

//--------------------------------------------------------------------------------------------------

bool HappensBeforeBase::defined_on_prefix(const index_t i) const
{
   return i <= mHB.size() - 1;
}

//--------------------------------------------------------------------------------------------------

std::string HappensBeforeBase::name()
{
   return "HappensBefore";
}

//--------------------------------------------------------------------------------------------------

std::string HappensBeforeBase::tabs()
{
   return "\t\t\t";
}

//--------------------------------------------------------------------------------------------------

std::string HappensBeforeBase::outputname()
{
   std::string outputname = tabs();
   outputname += text_color(name(), utils::io::Color::YELLOW);
   return outputname;
}

//--------------------------------------------------------------------------------------------------

bool HappensBeforeBase::happens_before(const index_t i1, const VectorClock& clock2) const
{
   const auto tid = boost::apply_visitor(program_model::get_tid(), mE[i1].instr());
   return clock2[tid] >= i1;
}

//--------------------------------------------------------------------------------------------------

const VectorClock& HappensBeforeBase::previous_by(const program_model::Thread::tid_t& tid) const
{
   const auto tid_index = boost::apply_visitor(program_model::get_tid(), mE[mIndex].instr());
   return tid_index == tid ? mHB[mHB[mIndex][tid]] : mFrontier[tid];
}

//--------------------------------------------------------------------------------------------------

} // end namespace exploration
