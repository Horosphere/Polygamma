#include "BufferSingular.hpp"

#include <cstring>
#include <iostream>
#include <limits>
#include <typeinfo>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#include "../media/io.h"
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
		std::free(playdata->samples);
	}
}
BufferSingular* BufferSingular::fromFile(std::string fileName,
    std::string* const error) noexcept
{
	DEBUG_TIMER_BEGIN;

	char const* errstr = nullptr;
	struct Media* media = new Media;
	Media_init(media);
	media->sampleFormat = SAMPLE_FORMAT;
	if (!Media_load_file(media, fileName.c_str(), &errstr))
	{
		*error = std::string(errstr);
		delete media;
		return nullptr;
	}
	BufferSingular* buffer = new BufferSingular(media->channelLayout);
	buffer->sampleRate = media->sampleRate;
	buffer->title = fileName;
	for (std::size_t i = 0; i < buffer->nAudioChannels(); ++i)
	{
		buffer->audio[i] = Vector<double>(media->nSamples, (double*) media->samples[i]);
	}
	DEBUG_TIMER_END("[Debug] Time: ");
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
	DEBUG_TIMER_BEGIN;

	if (!playdata)
	{
		playdata = new Media;
		Media_init(playdata);
		loadToMedia(playdata);
	}
	loadToMedia(playdata);
	char const* errstr = nullptr;
	if (!Media_save_file(playdata, fileName.c_str(), &errstr))
	{
		*error = std::string(errstr);
		return false;
	}

	DEBUG_TIMER_END("[Debug] Time: ");
	return true;
}

void BufferSingular::play() throw(PythonException)
{
	Buffer::play();
	if (!playdata)
	{
		playdata = new Media;
		Media_init(playdata);
		loadToMedia(playdata);
	}
	if (!media_open(playdata))
	{
		std::cout << "Unable to open media" << std::endl;
	}
	media_play(playdata);
}

void BufferSingular::loadToMedia(struct Media* const m) const noexcept
{
	m->nChannels = nAudioChannels();
	m->samples = (uint8_t**) std::malloc(sizeof(uint8_t*) * m->nChannels);
	for (std::size_t i = 0; i < m->nChannels; ++i)
	{
		m->samples[i] = (uint8_t*) audio[i].getData();
	}
	m->sampleFormat = SAMPLE_FORMAT;
	m->channelLayout = channelLayout;
	m->sampleRate = timeBase();
	m->nSamples = duration();
	m->cursor = 0;
}

} // namespace pg
