
#include "dfs.hpp"

namespace exploration
{
    DFSState::DFSState()
    : mDone() { }
    
    void DFSState::add_to_done(const Thread::tid_t& tid)
    {
        mDone.insert(tid);
    }
    
    Tids DFSState::undone(const Tids& T) const
    {
        Tids Undone{};
        if (!T.empty()) {
            std::set_difference(
                T.begin(), T.end(),
                mDone.begin(), mDone.end(),
                std::inserter(Undone, Undone.end())
            );
        }
        DEBUGFNL("\t\tDFSState", "undone", T, " = " << T << " \\ " << mDone << " = " << Undone);
        return Undone;
    }
    
    std::ostream& operator<<(std::ostream& os, const DFSState& s)
    {
        os << "done=" << s.mDone;
        return os;
    }
} // end namespace exploration
