#ifndef _POLYGAMMA_MEDIA_PLAYBACK_H__
#define _POLYGAMMA_MEDIA_PLAYBACK_H__

#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

struct Media
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

bool audio_play(struct Media* const);
void audio_stop(struct Media* const);

#endif // !_POLYGAMMA_MEDIA_PLAYBACK_H__
