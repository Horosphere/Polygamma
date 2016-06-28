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
	bool isSystemLevel() const noexcept;

	/**
	 * 0 = System level
	 * 1 = User level
	 */
	int level;
	std::string str;
};

} // namespace pg

// pg::Script
inline pg::Script::Script(): level(0), str()
{
}

inline pg::Script::Script(std::string const& str): level(0), str(str)
{
}

inline pg::Script::operator std::string() const noexcept
{
	return str;
}
inline bool pg::Script::isSystemLevel() const noexcept
{
	return level == 0;
}

#endif // !_POLYGAMMA_CORE_SCRIPT_HPP__
