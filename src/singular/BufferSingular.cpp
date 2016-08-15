#include "BufferSingular.hpp"

#include <cstring>
#include <iostream>
#include <limits>
#include <typeinfo>

#include <boost/timer/timer.hpp>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#include "../media/playback.h"
}

#include "../core/text.hpp"

namespace pg
{



// Implementations

BufferSingular::~BufferSingular()
{
	if (playdata)
	{
		delete playdata;
		delete[] playdata->samples;
	}
}
BufferSingular* BufferSingular::fromFile(std::string fileName,
    std::string* const error) noexcept
{
	char const* errstr = nullptr;
	struct Media* media = new Media;
	Media_init(media);
	media->sampleFormat = SAMPLE_FORMAT;
	if (!Media_load_file(media, fileName.c_str(), &errstr))
	{
		*error = std::string(errstr);
		return nullptr;
	}
	BufferSingular* buffer = new BufferSingular(media->channelLayout);
	buffer->sampleRate = media->sampleRate;
	buffer->title = fileName;
	for (std::size_t i = 0; i < buffer->nAudioChannels(); ++i)
	{
		buffer->audio[i] = Vector<double>(media->nSamples, (double*) media->samples[i]);
	}
	return buffer;
}
BufferSingular* BufferSingular::create(ChannelLayout cl,
                                       std::size_t sampleRate, std::size_t duration, std::string* const error)
noexcept
{
	std::cout << "[Ker] Creating BufferSingular with channel layout "
	          << channelLayoutName(cl)
	          << ", Sample Rate: " << sampleRate
	          << ", Duration: " << duration
	          << std::endl;
	if (duration == 0)
	{
		*error = "Duration too short";
		return nullptr;
	}
	if (std::find(std::begin(SAMPLE_RATES), std::end(SAMPLE_RATES), sampleRate) ==
	    std::end(SAMPLE_RATES))
	{
		*error = "Invalid sample rate";
		return nullptr;
	}

	BufferSingular* buffer = new BufferSingular(cl);
	buffer->sampleRate = sampleRate;
	buffer->title = "Untitled";
	for (auto& channel: buffer->audio)
		channel = Vector<real>(duration, 0.0);
	return buffer;
}
bool BufferSingular::saveToFile(std::string fileName,
                                std::string* const error) const noexcept
{
	return false;
}

void BufferSingular::play() throw(PythonException)
{
	Buffer::play();
	if (!playdata)
	{
		playdata = new Media;
		Media_init(playdata);
		playdata->nChannels = nAudioChannels();
		playdata->samples = new uint8_t*[playdata->nChannels];
		for (std::size_t i = 0; i < playdata->nChannels; ++i)
		{
			playdata->samples[i] = (uint8_t*) audio[i].getData();
		}
		playdata->sampleFormat = SAMPLE_FORMAT;
		playdata->channelLayout = channelLayout;
		playdata->sampleRate = timeBase();
		playdata->nSamples = duration();
		playdata->cursor = 0;
	}
	if (!media_open(playdata))
	{
		std::cout << "Unable to open media" << std::endl;
	}
	media_play(playdata);
}

} // namespace pg
