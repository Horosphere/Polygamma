#ifndef _POLYGAMMA_CORE_SCRIPT_HPP__
#define _POLYGAMMA_CORE_SCRIPT_HPP__

#include <string>

namespace pg
{
	
/**
 * @brief Container for a python command.
 */
struct Script
{
	Script();
	Script(std::string const&);

	operator std::string() const noexcept;

	std::string str;
};

} // namespace pg

// pg::Script
inline pg::Script::Script()
{
}

inline pg::Script::Script(std::string const& str): str(str)
{
}

inline pg::Script::operator std::string() const noexcept
{
	return str;
}

#endif // !_POLYGAMMA_CORE_SCRIPT_HPP__
