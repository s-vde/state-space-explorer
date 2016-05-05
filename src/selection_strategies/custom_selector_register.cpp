
#include "custom_selector_register.hpp"
#include "error.hpp"
#include "non_preemptive.hpp"
#include "sleep_sets.hpp"

namespace scheduler
{
    SelectorPtr custom_selector_factory(const std::string& tag)
    {
        if (tag == "SleepSets") {
            return std::make_unique<Selector<SleepSets>>();
        } else {
            ERROR("custom_selector_factory", "no selector registered under " << tag);
            return std::make_unique<Selector<NonPreemptive>>();
        }
    }
} // end namespace scheduler
