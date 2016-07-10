#ifndef _POLYGAMMA_CORE_TEXT_HPP__
#define _POLYGAMMA_CORE_TEXT_HPP__

#include <map>
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

std::map<ChannelLayout, std::string> const channelNames = boost::assign::map_list_of
    (AV_CH_LAYOUT_MONO, "Mono")
    (AV_CH_LAYOUT_STEREO, "Stereo")
    (AV_CH_LAYOUT_2POINT1, "2 Point 1")
    (AV_CH_LAYOUT_2_1, "2 1")
    (AV_CH_LAYOUT_SURROUND, "Surround")
    (AV_CH_LAYOUT_3POINT1, "3 Point 1")
    (AV_CH_LAYOUT_4POINT0, "4 Point 0")
    (AV_CH_LAYOUT_4POINT1, "4 POint 1")
    (AV_CH_LAYOUT_2_2, "2 2")
    (AV_CH_LAYOUT_QUAD, "Quad")
    (AV_CH_LAYOUT_5POINT0, "5 Point 0")
    (AV_CH_LAYOUT_5POINT1, "5 Point 1")
    (AV_CH_LAYOUT_5POINT0_BACK, "5 Point 0 Back")
    (AV_CH_LAYOUT_5POINT1_BACK, "5 Point 1 Back")
    (AV_CH_LAYOUT_6POINT0, "6 Point 0")
    (AV_CH_LAYOUT_6POINT0_FRONT, "6 Point 0 Front")
    (AV_CH_LAYOUT_HEXAGONAL, "Hexagonal")
    (AV_CH_LAYOUT_6POINT1, "6 Point 1")
    (AV_CH_LAYOUT_6POINT1_BACK, "6 Point 1 Back")
    (AV_CH_LAYOUT_6POINT1_FRONT, "6 Point 1 Front")
    (AV_CH_LAYOUT_7POINT0, "7 Point 0")
    (AV_CH_LAYOUT_7POINT0_FRONT, "7 Point 0 Front")
    (AV_CH_LAYOUT_7POINT1, "7 Point 1")
    (AV_CH_LAYOUT_7POINT1_WIDE, "7 Point 1 Wide")
    (AV_CH_LAYOUT_7POINT1_WIDE_BACK, "7 Point 1 Wide Back")
    (AV_CH_LAYOUT_OCTAGONAL, "Octagonal")
    (AV_CH_LAYOUT_HEXADECAGONAL, "Hexadecagonal")
    (AV_CH_LAYOUT_STEREO_DOWNMIX, "Stereo Downmix");

std::string timePointToString(std::size_t duration,
                             std::size_t sampleRate) noexcept;
std::size_t stringToTimePoint(std::string string,
                             std::size_t sampleRate) throw(PythonException);

} // namespace pg

// Implementations

inline std::string
pg::timePointToString(std::size_t duration, std::size_t sampleRate) noexcept
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
pg::stringToTimePoint(std::string string, std::size_t sampleRate) throw(PythonException) 
{
	std::vector<std::string> tokens;

	boost::algorithm::split(tokens, string, boost::algorithm::is_any_of(":"));

	switch(tokens.size())
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

#endif // !_POLYGAMMA_CORE_TEXT_HPP__
