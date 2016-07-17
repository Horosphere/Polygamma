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
	enum Level
	{
		System,
		UI,
		User
	};
	Script();
	Script(std::string const&);

	operator std::string() const noexcept;

	Level level;
	std::string str;
};

} // namespace pg

// pg::Script
inline pg::Script::Script(): level(UI), str()
{
}

inline pg::Script::Script(std::string const& str): level(UI), str(str)
{
}

inline pg::Script::operator std::string() const noexcept
{
	return str;
}

#endif // !_POLYGAMMA_CORE_SCRIPT_HPP__
