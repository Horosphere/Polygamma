#ifndef _POLYGAMMA_CORE_CONFIGURATION_HPP__
#define _POLYGAMMA_CORE_CONFIGURATION_HPP__

#include <string>
#include <tuple>

#include <boost/signals2.hpp>

namespace pg 
{

class Configuration final
{
public:
	/**
	 * Colour stored in the format
	 * 0xAABBGGRR
	 */
	typedef uint32_t Colour32;
	/**
	 * @brief Sets the configuration to its default values.
	 */
	Configuration();

	void setFileName(std::string) noexcept;
	/**
	 * @brief Loads the configuration from the given XML file.
	 * @return True if the file is accessible. False otherwise.
	 */
	bool loadFile();
	void saveFile();
	/**
	 * @brief Notifies all listener that the configuration has been updated.
	 */
	void update();
	/**
	 * @brief Registers a listener that will be notified (calling the
	 *	operator()() member function) when the configuration changes.
	 * @tparam Listener must have operator()() implemented.
	 */
	template <typename Listener>
	void registerUpdateListener(Listener);


	// The naming of the configurations must be consistent with
	// ui/DialogPreferences.hpp.
	Colour32 uiBG;
	Colour32 uiTerminalBG;
	Colour32 uiWaveformBG;
	Colour32 uiWaveformCore;
	Colour32 uiWaveformEdge;

private:
	boost::signals2::signal<void ()> signalChanged;
	std::string fileName;
};
/**
 * @brief Initialises the configuration.
 * @param[in] fileName Path to the configuration file
 * @return true if successful. false otherwise
 */
// bool init(std::string fileName);

} // namespace pg::config

// Implementations

inline void pg::Configuration::setFileName(std::string fileName) noexcept
{
	this->fileName = fileName;
}
inline void pg::Configuration::update()
{
	signalChanged();
	saveFile();
}
template <typename Listener> inline void
pg::Configuration::registerUpdateListener(Listener listener)
{
	signalChanged.connect(listener);
}
#endif // !_POLYGAMMA_CORE_CONFIGURATION_HPP__
