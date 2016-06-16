#ifndef _POLYGAMMA_CORE_CONFIGURATION_HPP__
#define _POLYGAMMA_CORE_CONFIGURATION_HPP__

#include <string>
#include <tuple>

namespace pg 
{

struct Configuration final
{
	/**
	 * Colour stored in the format
	 * 0xAARRGGBB
	 */
	typedef uint32_t Colour32;
	/**
	 * @brief Sets the configuration to its default values.
	 */
	Configuration();

	// The naming of the configurations must be consistent with
	// ui/DialogPreferences.hpp.
	Colour32 uiColourBG;
	Colour32 uiTerminalBG;
};
/**
 * @brief Initialises the configuration.
 * @param[in] fileName Path to the configuration file
 * @return true if successful. false otherwise
 */
// bool init(std::string fileName);

} // namespace pg::config

#endif // !_POLYGAMMA_CORE_CONFIGURATION_HPP__
