
#include "bound.hpp"

namespace exploration
{
    BoundState::BoundState(const int boundvalue)
    : mBoundValue(boundvalue) { }
    
    int BoundState::boundvalue() const
    {
        return mBoundValue;
    }
    
    void BoundState::set_boundvalue(const int bound_value)
    {
        mBoundValue = bound_value;
    }
    
    std::string BoundState::to_string() const
    {
        return std::to_string(mBoundValue);
    }
    
    std::ostream& operator<<(std::ostream& os, const BoundState& s)
    {
        os << "bval=" << s.boundvalue();
        return os;
    }
} // end namespace exploration