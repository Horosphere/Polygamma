#include "playback.h"

#include <stdio.h>

#include <libavutil/channel_layout.h>

void audio_callback(struct Audio* m, uint8_t* stream, int len)
{
	size_t spc = len / (m->nChannels * sizeof(short)); // Samples/Channel
	swr_convert(m->swrContext,
	            (uint8_t**) &stream, spc,
	            (uint8_t const**) m->samples, spc);
	size_t batchSize = av_get_bytes_per_sample(m->sampleFormat) * spc;
	for (size_t j = 0; j < m->nChannels; ++j)
	{
		m->samples[j] += batchSize;
	}
	m->cursor += batchSize;
}
bool audio_play(struct Audio* const m)
{
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
	SDL_PauseAudioDevice(m->audioDevice, 0);

	return true;
}
void audio_stop(struct Audio* const m)
{
	swr_free(&m->swrContext);
	SDL_CloseAudioDevice(m->audioDevice);
}
