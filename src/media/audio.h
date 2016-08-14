#ifndef _POLYGAMMA_MEDIA_AUDIO_H__
#define _POLYGAMMA_MEDIA_AUDIO_H__

#include <SDL2/SDL.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

struct Audio 
{
	uint8_t const** samples;
	enum AVSampleFormat sampleFormat;
	uint64_t channelLayout;
	size_t nChannels;
	unsigned int sampleRate;
	size_t nSamples;
	size_t cursor;

	// Initialised by play routine
	struct SwrContext* swrContext;
	SDL_AudioDeviceID audioDevice;
};

void Audio_init(struct Audio* const);
#endif // !_POLYGAMMA_MEDIA_AUDIO_H__
