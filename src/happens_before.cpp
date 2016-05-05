
#include "happens_before.hpp"
#include "algo.hpp"
#include "color_output.hpp"

namespace exploration
{
    Frontier::Frontier(const unsigned int nr_threads)
    : mFrontier(nr_threads, VectorClock(nr_threads)) { }
    
    /**
     @note Like std::vector[], this subscript operator does not throw if
     mFrontier.size() > i and yields undefined behaviour otherwise.
     */
    const VectorClock& Frontier::operator[](const index_t i) const
    {
        return mFrontier[i];
    }
    
    /**
     @note Like std::vector[], this function does not throw if
     mFrontier.size() > i and yields undefined behaviour otherwise.
     */
    void Frontier::set(const index_t i, const VectorClock& node)
    {
        mFrontier[i] = node;
    }
    
    /**
     @note Like std::vector[], this function does not throw if
     mFrontier.size() > i1 and mFrontier.size() > i2 and yields undefined 
     behaviour otherwise.
     */
    void Frontier::set(const index_t i1, const index_t i2, const VectorClock::value_t& val)
    {
        mFrontier[i1].set(i2, val);
    }
    
    /**
     @note Like std::vector[], this function does not throw if
     mFrontier.size() > i and yields undefined behaviour otherwise.
     */
    const VectorClock::value_t& Frontier::own(const index_t i) const
    {
        return (*this)[i][i];
    }
    
    /**
     @note Like std::vector[], this functions does not throw if
     mFrontier.size() > i and yields undefined behaviour otherwise.
     */
	void Frontier::set_own(const index_t i, const VectorClock::value_t& val)
    {
        set(i, i, val);
    }
    
    void Frontier::reset()
    {
        const unsigned int nr_threads = mFrontier.size();
        mFrontier.clear();
        mFrontier = Frontier_t(nr_threads, VectorClock(nr_threads));
    }

    Tids Frontier::active_threads() const
    {
        Tids Active{};
        for (Thread::tid_t tid = 0; tid < mFrontier.size(); ++tid) {
            if (own(tid) > 0) { Active.insert(tid); }
        }
        return Active;
    }
	
	bool HappensBeforeBase::happens_before(const index_t i1, const index_t i2) const
	{
		/// @pre defined_on_prefix(max(i1,i2))
		assert(defined_on_prefix(std::max(i1,i2)));
		return happens_before(i1, (*this)[i2]);
	}
	
	void HappensBeforeBase::transitive_reduction(const index_t i1, VectorClock& clock2) const
	{
		/// @pre happens_before(i1, clock2)
		assert(happens_before(i1, clock2));
		clock2.filter_values_greater_than((*this)[i1]);
	}
	
	void HappensBeforeBase::thread_transitive_reduction(const index_t i, const Thread::tid_t& tid, VectorClock& clock) const
	{
		/// @pre frontier_valid_for(i)
		assert(frontier_valid_for(i));
		clock.filter_values_greater_than(previous_by(tid));
		clock.set(tid, 0);
	}
	
	VectorClock::Indices_t HappensBeforeBase::thread_transitive_relation(
		const index_t i, const index_t ifrom, const Thread::tid_t tid) const
	{
		/// @pre frontier_valid_for(i)
		assert(frontier_valid_for(i));
		return previous_by(tid).indices_values_greater_than(ifrom);
	}
	
	VectorClock::Values_t HappensBeforeBase::incomparable_after(
		const index_t i1, const index_t i2) const
	{
		VectorClock::Values_t Incomparable{};
		for (int j = i1+1; j < i2; ++j) {
			if (!happens_before(i1, j)) {
				Incomparable.insert(j);
			}
		}
		DEBUGFNL("\t" << outputname(), "incomparable_after", "[" << i1 << "]", " = " << Incomparable);
		return Incomparable;
	}
	
	VectorClock::Values_t HappensBeforeBase::front(const VectorClock::Indices_t& subseq) const
	{
		VectorClock::Values_t Front{};
		if (!subseq.empty()) {
			VectorClock first_seen(mE.nr_threads());
			VectorClock last_seen(mE.nr_threads());
			for (const auto& i : subseq) {
				const Thread::tid_t tid = mE[i].instr().tid();
				const VectorClock& C = (*this)[i];
				if (first_seen[tid] == 0) {
					auto seen_before = utils::algo::find_if_with_index(
						C.begin(), C.end(),
						[&first_seen, &last_seen] (const auto& tid_, const auto& val) {
							return
								last_seen[tid_] > 0 &&
								first_seen[tid_] <= val &&
								val <= last_seen[tid_];
						}
					);
					if (seen_before != C.end()) {
						DEBUGNL(tabs() << "\t\t" << i << " notin Front");
					} else {
						DEBUGNL(tabs() << "\t\t" << i << " in Front");
						Front.insert(i);
					}
					first_seen.set(tid, i);
				} else {
					DEBUGNL(tabs() << "\t\t" << i << " notin Front : not first of " << tid);
					/// @invariant last_seen[tid] == C[tid]
					/// (i.e. subsequence of Transitions by tid in subseq
					/// does not skip Transitions by tid).
					assert(last_seen[tid] == C[tid]);
				}
				last_seen.set(tid, i);
			}
		}
		DEBUGFNL("\t" << outputname(), "front", subseq, " = " << Front);
		return Front;
	}
	
	void HappensBeforeBase::pop_back()
	{
		mHB.pop_back();
		mIndex = 0;
	}
	
	void HappensBeforeBase::reset()
	{
		mFrontier.reset();
		mIndex = 0;
	}
	
	void HappensBeforeBase::restore(const index_t i)
	{
		/// @pre not_restored() && frontier_valid_for(index-1) (impl. defined_on_prefix(i-1)).
		assert(not_restored() && frontier_valid_for(i-1));
		update_frontier(mE[i], mHB[i]);
		/// @post defined_on_prefix(i) && frontier_valid_for(i) (impl. invariant1)
		assert(defined_on_prefix(i) && frontier_valid_for(i));
	}
	
	Tids HappensBeforeBase::tids(const VectorClock::Values_t& indices) const
	{
		Tids T{};
		std::transform(
			indices.begin(), indices.end(),
			std::inserter(T, T.end()),
			[this] (const auto& i) { return mE[i].instr().tid(); }
		);
		return T;
	}
	
	const VectorClock& HappensBeforeBase::operator[](const index_t i) const
	{
		return mHB[i];
	}
	
	void HappensBeforeBase::update_frontier(const Transition<State>& t, const VectorClock& clock)
	{
		mFrontier.set(t.instr().tid(), clock);
		mFrontier.set_own(t.instr().tid(), t.index());
		++mIndex;
	}
	
	VectorClock::index_t HappensBeforeBase::max_dependent(
		const index_t i, const Instruction& instr, const bool ttr, VectorClock C) const
	{
		DEBUGFNL("\t" << outputname(), "max_dependent", "[" << i << "], " << instr, "");
		if (ttr) { thread_transitive_reduction(i, instr.tid(), C); }
		C.set(instr.tid(), 0); // exclude instr.tid-dependencies
		DEBUG(" = " << C.max());
		return C.max();
	}
	
	VectorClock::Indices_t HappensBeforeBase::covering(
		const index_t i, const Instruction& instr, VectorClock C) const
	{
		thread_transitive_reduction(i, instr.tid(), C);
		C.set(instr.tid(), 0); // exclude instr.tid-dependencies
		VectorClock::Indices_t Covering{};
		VectorClock::value_t j = 0;
		// in every loop-iteration an entry in C is set to 0.
		while (j = C.max(), j > 0) {
			Covering.insert(j);
			transitive_reduction(j, C);
			C.set(mE[j].instr().tid(), 0);
		}
		DEBUGFNL("\t" << outputname(), "covering", "[" << i << "], " << instr, " = " << Covering);
		return Covering;
	}
	
	bool HappensBeforeBase::restored() const
	{
		return mIndex == mHB.size()-1;
	}
	
	bool HappensBeforeBase::not_restored() const
	{
		return mIndex < mHB.size()-1;
	}
	
	bool HappensBeforeBase::frontier_valid_for(const index_t i) const
	{
		return mIndex == i;
	}
	
	bool HappensBeforeBase::defined_on_prefix(const index_t i) const
	{
		return i <= mHB.size()-1;
	}
	
	std::string HappensBeforeBase::name()
	{
		return "HappensBefore";
	}
	
	std::string HappensBeforeBase::tabs()
	{
		return "\t\t\t";
	}
	
	std::string HappensBeforeBase::outputname()
	{
		std::string outputname = tabs();
		outputname += text_color(name(), utils::io::Color::YELLOW);
		return outputname;
	}
	
	bool HappensBeforeBase::happens_before(const index_t i1, const VectorClock& clock2) const
	{
		return clock2[mE[i1].instr().tid()] >= i1;
	}
	
	const VectorClock& HappensBeforeBase::previous_by(const Thread::tid_t& tid) const
	{
		return
		mE[mIndex].instr().tid() == tid
		? mHB[mHB[mIndex][tid]]
		: mFrontier[tid];
	}
} // end namespace exploration
