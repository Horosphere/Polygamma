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

// Implementations


inline Script::Script(): level(UI), str()
{
}

inline Script::Script(std::string const& str): level(UI), str(str)
{
}

inline Script::operator std::string() const noexcept
{
	return str;
}

} // namespace pg


#endif // !_POLYGAMMA_CORE_SCRIPT_HPP__
