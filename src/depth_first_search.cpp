
#include "depth_first_search.hpp"

namespace exploration
{
//--------------------------------------------------------------------------------------------------
    
void dfs_state::add_to_done(const program_model::Thread::tid_t& tid)
{
   mDone.insert(tid);
}

//--------------------------------------------------------------------------------------------------
    
program_model::Tids dfs_state::undone(const program_model::Tids& tids) const
{
   program_model::Tids undone;
   std::set_difference(tids.begin(), tids.end(), mDone.begin(), mDone.end(),
                       std::inserter(undone, undone.end()));
   DEBUGF("dfs_state", "undone", tids, " = " << tids << " \\ " << mDone << " = " << undone << "\n");
   return undone;
}

//--------------------------------------------------------------------------------------------------
    
std::ostream& operator<<(std::ostream& os, const dfs_state& s)
{
   os << "done=" << s.mDone;
   return os;
}

//--------------------------------------------------------------------------------------------------
} // end namespace exploration
