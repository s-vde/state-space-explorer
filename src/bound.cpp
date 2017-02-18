
#include "bound.hpp"

namespace exploration
{
//--------------------------------------------------------------------------------------------------

BoundState::BoundState(const int boundvalue)
: mBoundValue(boundvalue) 
{ 
}
    
//--------------------------------------------------------------------------------------------------

int BoundState::bound_value() const
{
   return mBoundValue;
}

//--------------------------------------------------------------------------------------------------
    
void BoundState::set_bound_value(const int bound_value)
{
   mBoundValue = bound_value;
}

//--------------------------------------------------------------------------------------------------  
    
std::string BoundState::to_string() const
{
   return std::to_string(mBoundValue);
}

//--------------------------------------------------------------------------------------------------
    
std::ostream& operator<<(std::ostream& os, const BoundState& state)
{
   os << "bval=" << state.bound_value();
   return os;
}

//--------------------------------------------------------------------------------------------------
} // end namespace exploration