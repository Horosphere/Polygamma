#ifndef _POLYGAMMA_CORE_COMMAND_HPP__
#define _POLYGAMMA_CORE_COMMAND_HPP__

#include <string>

namespace pg
{

struct Command
{
	Command();
	Command(std::string const&);

	std::string str;
};

} // namespace pg


// Implementations

inline pg::Command::Command()
{
}

inline pg::Command::Command(std::string const& str): str(str)
{
}

#endif // !_POLYGAMMA_CORE_COMMAND_HPP__
