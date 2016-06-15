#ifndef _POLYGAMMA_CORE_CONFIGURATION_HPP__
#define _POLYGAMMA_CORE_CONFIGURATION_HPP__

#include <string>
#include <tuple>

namespace pg 
{

struct Configuration final
{
	typedef uint32_t Colour32;
	/**
	 * @brief Sets the configuration to its default values.
	 */
	Configuration();

		
	std::string uiLanguage;

	Colour32 uiBaseColour;
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
