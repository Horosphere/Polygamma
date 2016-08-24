#ifndef _POLYGAMMA_CORE_TEXT_HPP__
#define _POLYGAMMA_CORE_TEXT_HPP__

#include <vector>

extern "C"
{
#include <libavutil/channel_layout.h>
}
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "polygamma.hpp"
#include "python.hpp"

namespace pg
{

/*
 * Tuple elements:
 * 1. Channel layout stored in \ref ChannelLayout
 * 2. Human readable channel name stored in std::string
 * 3. Channel name exposed to scripts
 */
std::vector<boost::tuple<ChannelLayout, std::string, std::string>> const channelNames = boost::assign::tuple_list_of
    (AV_CH_LAYOUT_MONO, "Mono", "CH_LAYOUT_MONO")
    (AV_CH_LAYOUT_STEREO, "Stereo", "CH_LAYOUT_STEREO")
    (AV_CH_LAYOUT_STEREO_DOWNMIX, "Stereo Downmix", "CH_LAYOUT_STEREO_DOWNMIX")
    (AV_CH_LAYOUT_2POINT1, "2 Point 1", "CH_LAYOUT_2POINT1")
    (AV_CH_LAYOUT_2_1, "2 1", "CH_LAYOUT_2_1")
    (AV_CH_LAYOUT_SURROUND, "Surround", "CH_LAYOUT_SURROUND")
    (AV_CH_LAYOUT_3POINT1, "3 Point 1", "CH_LAYOUT_3POINT1")
    (AV_CH_LAYOUT_4POINT0, "4 Point 0", "CH_LAYOUT_4POINT0")
    (AV_CH_LAYOUT_4POINT1, "4 POint 1", "CH_LAYOUT_4POINT1")
    (AV_CH_LAYOUT_2_2, "2 2", "CH_LAYOUT_2_2")
    (AV_CH_LAYOUT_QUAD, "Quad", "CH_LAYOUT_QUAD")
    (AV_CH_LAYOUT_5POINT0, "5 Point 0", "CH_LAYOUT_5POINT0")
    (AV_CH_LAYOUT_5POINT1, "5 Point 1", "CH_LAYOUT_5POINT1")
    (AV_CH_LAYOUT_5POINT0_BACK, "5 Point 0 Back", "CH_LAYOUT_5POINT0_BACK")
    (AV_CH_LAYOUT_5POINT1_BACK, "5 Point 1 Back", "CH_LAYOUT_5POINT1_BACK")
    (AV_CH_LAYOUT_6POINT0, "6 Point 0", "CH_LAYOUT_6POINT0")
    (AV_CH_LAYOUT_6POINT0_FRONT, "6 Point 0 Front", "CH_LAYOUT_6POINT0_FRONT")
    (AV_CH_LAYOUT_HEXAGONAL, "Hexagonal", "CH_LAYOUT_HEXAGONAL")
    (AV_CH_LAYOUT_6POINT1, "6 Point 1", "CH_LAYOUT_6POINT1")
    (AV_CH_LAYOUT_6POINT1_BACK, "6 Point 1 Back", "CH_LAYOUT_6POINT1_BACK")
    (AV_CH_LAYOUT_6POINT1_FRONT, "6 Point 1 Front", "CH_LAYOUT_6POINT1_FRONT")
    (AV_CH_LAYOUT_7POINT0, "7 Point 0", "CH_LAYOUT_7POINT0")
    (AV_CH_LAYOUT_7POINT0_FRONT, "7 Point 0 Front", "CH_LAYOUT_7POINT0_FRONT")
    (AV_CH_LAYOUT_7POINT1, "7 Point 1", "CH_LAYOUT_7POINT1")
    (AV_CH_LAYOUT_7POINT1_WIDE, "7 Point 1 Wide", "CH_LAYOUT_7POINT1_WIDE")
    (AV_CH_LAYOUT_7POINT1_WIDE_BACK, "7 Point 1 Wide Back", "CH_LAYOUT_7POINT1_WIDE_BACK")
    (AV_CH_LAYOUT_OCTAGONAL, "Octagonal", "CH_LAYOUT_OCTAGONAL");
// Hexadecagonal layout has 47 channels
//    (AV_CH_LAYOUT_HEXADECAGONAL, "Hexadecagonal", "CH_LAYOUT_HEXADECAGONAL")

std::string channelLayoutName(ChannelLayout const& channelLayout) noexcept;

std::string timePointToString(std::size_t duration,
                              std::size_t sampleRate) noexcept;
/**
 * @brief Converts a string to samples
 */
std::size_t stringToTimePoint(std::string string,
                              std::size_t sampleRate) throw(PythonException);
/**
 * @brief Converts a string to seconds
 */
std::size_t stringToTimePoint(std::string string) throw(PythonException);


// Implementations

inline std::string channelLayoutName(ChannelLayout const& cl) noexcept
{
	for (auto const& channelName: channelNames)
		if (boost::get<0>(channelName) == cl)
			return boost::get<1>(channelName);

	assert(false && "Unrecognised channel name");
	return "";
}
inline std::string
timePointToString(std::size_t duration, std::size_t sampleRate) noexcept
{
	std::string result = std::to_string(duration % sampleRate);
	duration /= sampleRate;
	if (duration)
	{
		result = std::to_string(duration % 60) + ':' + result;
		duration /= 60;
		if (duration)
		{
			result = std::to_string(duration % 60) + ':' + result;
			duration /= 60;
			if (duration) result = std::to_string(duration) + ':' + result;
		}
	}
	return result;
}

inline std::size_t
stringToTimePoint(std::string string, std::size_t sampleRate) throw(PythonException)
{
	std::vector<std::string> tokens;

	boost::algorithm::split(tokens, string, boost::algorithm::is_any_of(":"));

	switch (tokens.size())
	{
	case 0:
		return 0;
	case 1:
		return std::stoul(tokens[0]);
	case 2:
		return std::stoul(tokens[1]) + sampleRate * std::stoul(tokens[0]);
	case 3:
		return std::stoul(tokens[2]) +
		       sampleRate * std::stoul(tokens[1]) +
		       sampleRate * 60 * std::stoul(tokens[0]);
	case 4:
		return std::stoul(tokens[3]) +
		       sampleRate * std::stoul(tokens[2]) +
		       sampleRate * 60 * std::stoul(tokens[1]) +
		       sampleRate * 60 * 60 * std::stoul(tokens[0]);
	default:
		throw PythonException{"Invalid duration", PythonException::ValueError};
	}
}
inline std::size_t
stringToTimePoint(std::string string) throw(PythonException)
{
	std::vector<std::string> tokens;

	boost::algorithm::split(tokens, string, boost::algorithm::is_any_of(":"));

	switch (tokens.size())
	{
	case 0:
		return 0;
	case 1:
		return std::stoul(tokens[0]);
	case 2:
		return std::stoul(tokens[1]) + 60 * std::stoul(tokens[0]);
	case 3:
		return std::stoul(tokens[2]) +
		       60 * std::stoul(tokens[1]) +
		       60 * 60 * std::stoul(tokens[0]);
	default:
		throw PythonException{"Invalid duration", PythonException::ValueError};
	}
}

} // namespace pg

#endif // !_POLYGAMMA_CORE_TEXT_HPP__
