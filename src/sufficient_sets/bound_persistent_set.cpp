
#include "bound_persistent_set.hpp"
#include "utils_io.hpp"

namespace exploration
{
    BoundPersistentState::BoundPersistentState(const int boundvalue)
    : BoundState(boundvalue)
    , mPending()
    , mBoundExceeded(false) { }
    
	void BoundPersistentState::add_to_pending(const Thread::tid_t& tid)
    {
        mPending.insert(tid);
    }
    
    void BoundPersistentState::add_to_pending(const Tids& tids)
    {
        std::set_union(
            mPending.begin(), mPending.end(),
            tids.begin(), tids.end(),
            std::inserter(mPending, mPending.end())
        );
    }
    
    const Tids& BoundPersistentState::pending() const
    {
        return mPending;
    }
    
    void BoundPersistentState::set_bound_exceeded()
    {
        mBoundExceeded = true;
    }
    
    bool BoundPersistentState::bound_exceeded() const
    {
        return mBoundExceeded;
    }
    
    std::string BoundPersistentState::to_string() const
    {
        std::string str = BoundState::to_string();
        str += " ";
        str += utils::io::to_string(mPending);
        str += (mBoundExceeded ? " bound_exceeded" : "");
        return str;
    }

    std::ostream& operator<<(std::ostream& os, const BoundPersistentState& s)
    {
        os << s.to_string();
        return os;
    }
	
	BoundPersistentOptimizations::BoundPersistentOptimizations(
		const bool transred,
		const bool alternative,
		const bool bound_opt,
		const sleep_t sleepsets)
	: mTRANSITIVE_REDUCTION(transred)
	, mALTERNATIVE_THREAD(alternative)
	, mBOUND_OPT(bound_opt)
	, mSLEEPSETS(sleepsets)
	{
		if (mSLEEPSETS != sleep_t::NEVER) {
			mBOUND_OPT = true;
		}
	}
	
	bool BoundPersistentOptimizations::TRANSITIVE_REDUCTION() const
	{
		return mTRANSITIVE_REDUCTION;
	}
	
	bool BoundPersistentOptimizations::ALTERNATIVE_THREAD() const
	{
		return mALTERNATIVE_THREAD;
	}
	
	bool BoundPersistentOptimizations::BOUND_OPT() const
	{
		return mBOUND_OPT;
	}
	
	BoundPersistentOptimizations::sleep_t BoundPersistentOptimizations::SLEEPSETS() const
	{
		return mSLEEPSETS;
	}
	
	std::string BoundPersistentOptimizations::path() const
	{
		std::string path{};
		path += (mTRANSITIVE_REDUCTION ? "TR" : "tr");
		path += (mALTERNATIVE_THREAD ? "-ALT" : "alt");
		path += (mBOUND_OPT ? "-B" : "b");
		if (mSLEEPSETS == NEVER)              { path += "-sNEV";    }
		else if (mSLEEPSETS == CONSERVATIVE)  { path += "-sCONS";   }
		return path;
	}
	
	BoundPersistentBase::BoundPersistentBase()
	: mState({ BoundPersistentState() })
 	, mOpt() { }
	
	void BoundPersistentBase::add_to_pool(Tids& pool) const
	{
		if (mState.back().bound_exceeded()) {
			for (const auto& pending : mState.back().pending()) {
				pool.insert(pending);
			}
		}
	}
	
	void BoundPersistentBase::update_after_exploration(const transition& t, SufficientSet& t_pre)
	{
		/// @pre mState.size() == t.index()+1 (i.e. did not yet pop_back())
		assert(mState.size() == t.index()+1);
		if (mState[t.index()].bound_exceeded()) {
			DEBUGF(
				outputname(), "update_after_exploration", to_string_pre_state(t),
				to_string_pre_state(t) << ".set_bound_exceeded()" << "\n"
			);
			mState[t.index()-1].set_bound_exceeded();
		}
		if (mOpt.SLEEPSETS() == mOpt.sleep_t::NEVER ||
			/* AND */	(mOpt.SLEEPSETS() == mOpt.sleep_t::CONSERVATIVE &&
						 mState[t.index()-1].bound_exceeded()))
		{
         const auto tid = boost::apply_visitor(program_model::get_tid(), t.instr());
			t_pre.wake_up(tid);
		}
	}
	
	void BoundPersistentBase::pop_back()
	{
		mState.pop_back();
	}
	
	bool BoundPersistentBase::adding_condition(const State& s, const SufficientSet& suf, const Thread::tid_t& tid) const
	{
		if (mOpt.SLEEPSETS() == mOpt.sleep_t::NEVER) {
			return s.is_enabled(tid);
		} else {
			return s.is_enabled(tid) && suf.sleepset().is_awake(tid);
		}
	}
	
	Tids BoundPersistentBase::adding_tids(const State& s, const SufficientSet& suf) const
	{
		if (mOpt.SLEEPSETS() == mOpt.sleep_t::NEVER) {
			return s.enabled();
		} else {
			return suf.sleepset().awake(s.enabled());
		}
	}
	
	const std::string BoundPersistentBase::name = "BoundPersistent";
	
	const std::string BoundPersistentBase::tabs = "\t\t\t";
	
	std::string BoundPersistentBase::outputname()
	{
		std::string outputname = tabs;
		outputname += text_color(name, utils::io::Color::GREEN);
		return outputname;
	}
	
} // end namespace exploration
