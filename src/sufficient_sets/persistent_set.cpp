
#include "persistent_set.hpp"

namespace exploration
{
    std::string Persistent::name()
    {
        return "Persistent";
    }
    
    std::string Persistent::path()
    {
        return name();
    }
    
    /**
     @brief Does nothing.
     */
    void Persistent::update_state(const execution& E, const transition& t) { }
    
    /**
     @brief Does nothing.
     */
    void Persistent::update_after_exploration(const transition&, SufficientSet&) { }
    
    /**
     @brief Does nothing.
     */
    void Persistent::add_to_pool(Tids&) { }
    
    /**
     @brief Always returns true.
     */
	bool Persistent::condition(const execution&, SufficientSet&, const Thread::tid_t&)
    {
        DEBUGF(outputname(), "condition", "", " = true\n");
        return true;
    }
    
    /**
     @brief Does nothing.
     */
    void Persistent::pop_back() { }
    
    std::string Persistent::tabs()
    {
        return "\t\t\t";
    }
    
    std::string Persistent::outputname()
    {
        std::string outputname = tabs();
		outputname += text_color(name(), utils::io::Color::GREEN);
        return outputname;
    }
} // end namespace exploration
