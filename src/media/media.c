#include "media.h"

#include <assert.h>

#include <libavformat/avformat.h>

void Media_init(struct Media* const media)
{
	assert(media);
	memset(media, 0, sizeof(struct Media));
}
bool Media_load_file(struct Media* const m,
                     char const* const fileName,
                     char const** const error)
{
	assert(m);
	bool flag = false;
	struct AVFormatContext* fc = NULL;

	if (avformat_open_input(&fc, fileName, NULL, NULL))
	{
		*error = "Unable to open file";
		goto complete;
	}
	if (avformat_find_stream_info(fc, NULL) < 0)
	{
		*error = "Unable to find stream within media";
		goto complete;
	}
	av_dump_format(fc, 0, fileName, 0);

	AVCodec* audioCodec = NULL;
	int audioStreamIndex = av_find_best_stream(fc, AVMEDIA_TYPE_AUDIO,
	                       -1, -1, &audioCodec, 0);
	if (audioStreamIndex < 0)
	{
		*error = "The file has no audio stream";
		goto complete;
	}
	struct AVStream* audioStream = fc->streams[audioStreamIndex];
	struct AVCodecContext* audioCC = audioStream->codec;
	audioCC->codec = audioCodec;
	if (avcodec_open2(audioCC, audioCodec, NULL))
	{
		*error = "Unable to open codec";
		goto complete;
	}
	m->nChannels = audioCC->channels;
	m->channelLayout = audioCC->channel_layout;
	m->sampleRate = audioCC->sample_rate;
	SwrContext* swrContext =
	  swr_alloc_set_opts(NULL,
	                     m->channelLayout, m->sampleFormat, m->sampleRate,
	                     audioCC->channel_layout, audioCC->sample_fmt, audioCC->sample_rate,
	                     0, NULL);
	if (!swrContext || swr_init(swrContext) < 0)
	{
		*error = "Unable to initialise SwrContext";
		goto complete;
	}

	bool planar = av_sample_fmt_is_planar(m->sampleFormat);
	size_t bps = av_get_bytes_per_sample(m->sampleFormat);

	uint8_t** sampleIn;
	if (planar)
	{
		m->samples = (uint8_t**) calloc(m->nChannels, sizeof(uint8_t const*));
		sampleIn = (uint8_t**) calloc(m->nChannels, sizeof(uint8_t const*));
	}
	else
	{
		m->samples = (uint8_t**) calloc(1, sizeof(uint8_t const*));
		sampleIn = (uint8_t**) calloc(1, sizeof(uint8_t const*));
	}

	AVFrame* frame = av_frame_alloc();
	if (!frame)
	{
		goto complete;
	}

	// Reading loop
	struct AVPacket packet;
	av_init_packet(&packet);
	m->nSamples = 0;
	while (!av_read_frame(fc, &packet))
	{
		if (packet.stream_index == audioStreamIndex)
		{
			while (packet.size > 0)
			{
				int gotFrame = 0;
				int dataSize = avcodec_decode_audio4(audioCC, frame,
				                                     &gotFrame, &packet);
				if (dataSize >= 0 && gotFrame)
				{
					packet.size -= dataSize;
					packet.data += dataSize;

					// Expand buffers
					m->cursor = m->nSamples;
					m->nSamples += frame->nb_samples;
					if (planar)
					{
						for (size_t i = 0; i < m->nChannels; ++i)
						{
							m->samples[i] = (uint8_t*) realloc(m->samples[i],
							                                   m->nSamples * bps);
							sampleIn[i] = m->samples[i] + m->cursor * bps;
						}
					}
					else
					{
						m->samples[0] = (uint8_t*) realloc(m->samples[0],
						                                   m->nSamples * m->nChannels * bps);
						sampleIn[0] = m->samples[0] + m->cursor * bps * m->nChannels;
					}
					swr_convert(swrContext,
					            sampleIn, m->nSamples - m->cursor,
					            (uint8_t const**) frame->extended_data, frame->nb_samples);
				}
				else
				{
					packet.size = 0;
					packet.data = NULL;
				}
			}
		}
		av_packet_unref(&packet);
	}

	flag = true;
complete:
	av_frame_free(&frame);
	free(sampleIn);
	if (!flag && m->samples)
	{
		if (planar)
			for (size_t i = 0; i < m->nChannels; ++i)
				free(m->samples[i]);
		else free(m->samples[0]);
		free(m->samples);
	}

	avformat_close_input(&fc);
	return flag;
}
