#ifndef _POLYGAMMA_MEDIA_MEDIA_H__
#define _POLYGAMMA_MEDIA_MEDIA_H__

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

struct Media
{
	uint8_t** samples;
	enum AVSampleFormat sampleFormat;
	uint64_t channelLayout;
	size_t nChannels;
	unsigned int sampleRate;
	size_t nSamples;
	size_t cursor;

	// Initialised by play routine
	struct SwrContext* swrContext;
	SDL_AudioDeviceID audioDevice;
	bool playing;
};

void Media_init(struct Media* const);

#endif // !_POLYGAMMA_MEDIA_MEDIA_H__
