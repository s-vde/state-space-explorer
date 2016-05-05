
#ifndef CUSTOM_SELECTOR_REGISTER_HPP_INCLUDED
#define CUSTOM_SELECTOR_REGISTER_HPP_INCLUDED

#include "selector.hpp"

namespace scheduler
{
    using SelectorPtr = std::unique_ptr<SelectorBase>;
    
    SelectorPtr custom_selector_factory(const std::string& tag);
    
} // end namespace scheduler

#endif
