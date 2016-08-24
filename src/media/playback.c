#include "playback.h"

#include <stdio.h>
#include <assert.h>

#include <libavutil/channel_layout.h>

void audio_callback(struct Media* m, uint8_t* stream, int len)
{
	assert(m);
	size_t spc = len / (m->nChannels * sizeof(short)); // Samples/Channel
	// Available sample/channel
	size_t spcA = spc > m->nSamples - m->cursor ?
		m->nSamples - m->cursor : spc;
	assert(spcA <= spc);

	swr_convert(m->swrContext,
	            (uint8_t**) &stream, spcA,
	            (uint8_t const**) m->samples, spcA);

	// Fill trailing space with zeroes
	if (spcA < spc)
	{
		size_t trailing = spcA * m->nChannels * sizeof(short);
		assert(trailing <= len);
		memset(stream + trailing, 0, len - trailing);
		m->playing = false;
		SDL_PauseAudioDevice(m->audioDevice, true);
		Media_set_cursor(m, 0);
	}

	// Advance the cursor
	size_t batchSize = av_get_bytes_per_sample(m->sampleFormat) * spcA;
	for (size_t j = 0; j < m->nChannels; ++j)
	{
		m->samples[j] += batchSize;
	}
	m->cursor += batchSize;
}
bool media_open(struct Media* const m)
{
	assert(m);
	struct SDL_AudioSpec specTarget;
	specTarget.freq = m->sampleRate;
	specTarget.format = AUDIO_S16SYS;
	specTarget.channels = m->nChannels;
	specTarget.silence = false;
	specTarget.samples = 1024 * m->nChannels;
	specTarget.callback = (SDL_AudioCallback) audio_callback;
	specTarget.userdata = m;

	struct SDL_AudioSpec spec;
	m->audioDevice = SDL_OpenAudioDevice(NULL, false,
	                                     &specTarget, &spec,
	                                     SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if (!m->audioDevice)
	{
		fprintf(stderr, "[SDL] %s\n", SDL_GetError());
		return false;
	}
	m->swrContext =
	  swr_alloc_set_opts(NULL,
	                     m->channelLayout, AV_SAMPLE_FMT_S16, spec.freq,
	                     m->channelLayout, m->sampleFormat, m->sampleRate,
	                     0, NULL);
	if (!m->swrContext)
	{
		SDL_CloseAudioDevice(m->audioDevice);
		fprintf(stderr, "Unable to allocate SwrContext\n");
		return false;
	}
	if (swr_init(m->swrContext) < 0)
	{
		fprintf(stderr, "Unable to initialise SwrContext\n");
		swr_free(&m->swrContext);
		SDL_CloseAudioDevice(m->audioDevice);
		return false;
	}
	return true;
}
void media_close(struct Media* const m)
{
	if (!m) return;
	swr_free(&m->swrContext);
	SDL_CloseAudioDevice(m->audioDevice);
	m->playing = false;
}

bool media_play(struct Media* const m)
{
	assert(m);
	SDL_PauseAudioDevice(m->audioDevice, false);
	m->playing = true;

	return true;
}
void media_stop(struct Media* const m)
{
	if (!m) return;
	SDL_PauseAudioDevice(m->audioDevice, true);
	m->playing = false;
}
