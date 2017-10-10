
#include "source_set.hpp"
#include "error.hpp"

namespace exploration
{
	bool Source::check_valid(const bool contains_locks) const
	{
		if (contains_locks) { ERROR(name(), "Assumes threads do not disable each other"); }
		return !contains_locks;
	}
	
    std::string Source::name()
    {
        return "Source";
    }
    
    std::string Source::path()
    {
        return name();
    }
    
    /**
     @brief Does nothing.
     */
    void Source::update_state(const execution& E, const transition& t) { }
    
    /**
     @brief Does nothing.
     */
    void Source::update_after_exploration(const transition&, SufficientSet&) { }
    
    /**
     @brief Does nothing.
     */
    void Source::add_to_pool(Tids&) { }
    
    /**
     @brief Always returns true.
     */
    bool Source::condition(const execution&, SufficientSet&, const Thread::tid_t&)
    {
        DEBUGF(outputname(), "condition", "", " = true\n");
        return true;
    }
    
    /**
     @brief Does nothing.
     */
    void Source::pop_back() { }
    
    std::string Source::tabs()
    {
        return "\t\t\t";
    }
    
    std::string Source::outputname()
    {
        std::string outputname = tabs();
		outputname += text_color(name(), utils::io::Color::GREEN);
        return outputname;
    }
} // end namespace exploration
