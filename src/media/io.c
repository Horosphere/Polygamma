#include "io.h"

#include <assert.h>
#include <libavformat/avformat.h>

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

	uint8_t** sampleOut;
	if (planar)
	{
		m->samples = (uint8_t**) calloc(m->nChannels, sizeof(uint8_t const*));
		sampleOut = (uint8_t**) malloc(m->nChannels * sizeof(uint8_t const*));
	}
	else
	{
		m->samples = (uint8_t**) calloc(1, sizeof(uint8_t const*));
		sampleOut = (uint8_t**) malloc(sizeof(uint8_t const*));
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
							sampleOut[i] = m->samples[i] + m->cursor * bps;
						}
					}
					else
					{
						m->samples[0] = (uint8_t*) realloc(m->samples[0],
						                                   m->nSamples * m->nChannels * bps);
						sampleOut[0] = m->samples[0] + m->cursor * bps * m->nChannels;
					}
					swr_convert(swrContext,
					            sampleOut, m->nSamples - m->cursor,
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
	free(sampleOut);
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
bool Media_save_file(struct Media const* const m,
                     char const* const fileName,
                     char const** const error)
{
	bool flag = false;
	struct AVOutputFormat* format = av_guess_format(NULL, fileName, NULL);
	if (!format)
	{
		*error = "Unable to find suitable format for given extension";
		goto complete;
	}
	AVCodec* audioCodec = avcodec_find_encoder(format->audio_codec);
	if (!audioCodec)
	{
		*error = "Unable to find audio encoder";
		goto complete;
	}
	assert(audioCodec->sample_fmts);
	struct AVCodecContext* audioCC = avcodec_alloc_context3(audioCodec);
	if (!audioCC)
	{
		*error = "Unable to allocate audio codec context";
		goto complete;
	}
	audioCC->sample_fmt = audioCodec->sample_fmts[0];
	audioCC->bit_rate = 128000; // TODO: Calculate automatically
	audioCC->sample_rate = m->sampleRate;
	audioCC->channels = m->nChannels;
	audioCC->channel_layout = m->channelLayout;
	if (avcodec_open2(audioCC, audioCodec, NULL) < 0)
	{
		*error = "Unable to open audio codec";
		goto complete;
	}
	SwrContext* swrContext =
	  swr_alloc_set_opts(NULL,
	                     audioCC->channel_layout, audioCC->sample_fmt, audioCC->sample_rate,
	                     m->channelLayout, m->sampleFormat, m->sampleRate,
	                     0, NULL);
	if (!swrContext || swr_init(swrContext) < 0)
	{
		*error = "Unable to initialise SwrContext";
		goto complete;
	}
	int bufferSize = av_samples_get_buffer_size(NULL,
	                 audioCC->channels,
	                 audioCC->frame_size,
	                 audioCC->sample_fmt, 0);
	if (bufferSize < 0)
	{
		*error = "Unable to obtain sample buffer size";
		goto complete;
	}
	FILE* file = fopen(fileName, "wb");
	if (!file)
	{
		*error = "Unable to write to destination";
		goto complete;
	}
	AVFrame* frame = av_frame_alloc();
	if (!frame)
	{
		*error = "Unable to allocate frame";
		goto complete;
	}
	frame->nb_samples = audioCC->frame_size;
	frame->format = audioCC->sample_fmt;
	frame->channel_layout = audioCC->channel_layout;
	uint8_t* buffer = av_malloc(bufferSize);
	if (!buffer)
	{
		*error = "Unable to allocate samples";
		goto complete;
	}
	if (avcodec_fill_audio_frame(frame, audioCC->channels, audioCC->sample_fmt,
	                             buffer, bufferSize, 0) < 0)
	{
		*error = "Unable to fill audio frame with samples";
		goto complete;
	}


	uint8_t** sampleOut;
	size_t bpsOut = av_get_bytes_per_sample(audioCC->sample_fmt);
	bool planarOut = av_sample_fmt_is_planar(audioCC->sample_fmt);
	if (planarOut)
	{
		sampleOut = (uint8_t**) malloc(audioCC->channels * sizeof(uint8_t*));
		for (size_t i = 0; i < audioCC->channels; ++i)
		{
			sampleOut[i] = buffer + audioCC->frame_size * i * bpsOut;
		}
	}
	else
	{
		sampleOut = (uint8_t**) malloc(sizeof(uint8_t*));
		sampleOut[0] = buffer;
	}

	uint8_t const** sampleIn;
	size_t bpsIn = av_get_bytes_per_sample(m->sampleFormat);
	bool planarIn = av_sample_fmt_is_planar(m->sampleFormat);
	if (planarIn)
		sampleIn = (uint8_t const**) calloc(m->nChannels, sizeof(uint8_t const*));
	else
		sampleIn = (uint8_t const**) calloc(1, sizeof(uint8_t const*));


	// Encode audio into frame
	size_t nAudioFrames = m->nSamples / audioCC->frame_size;

	/*
	 * FIXME: This routine may create a problem if the number of remaining
	 * samples is a fraction of audioCC->frame_size.
	 */
	assert(m->nSamples % audioCC->frame_size == 0);

	size_t iSample = 0;

	struct AVPacket packet;
	for (size_t iFrame = 0; iFrame < nAudioFrames; ++iFrame)
	{
		av_init_packet(&packet);
		packet.data = NULL; // Allocated by the encoder
		packet.size = 0;

		if (planarIn)
		{
			for (size_t i = 0; i < m->nChannels; ++i)
			{
				sampleIn[i] = m->samples[i] + iSample * bpsIn;
			}
		}
		else
		{
			sampleIn[0] = m->samples[0] + iSample * bpsIn * m->nChannels;
		}
		swr_convert(swrContext,
		            sampleOut, frame->nb_samples,
		            sampleIn, frame->nb_samples);

		int gotOutput;
		if (avcodec_encode_audio2(audioCC, &packet, frame, &gotOutput) < 0)
		{
			*error = "Unable to encode audio";
			goto complete;
		}
		if (gotOutput)
		{
			fwrite(packet.data, 1, packet.size, file);
			av_packet_unref(&packet);
		}

		iSample += frame->nb_samples;
	}

	flag = true;
complete:
	free(sampleIn);
	free(sampleOut);
	av_free(buffer);
	av_frame_free(&frame);
	swr_free(&swrContext);
	fclose(file);
	avcodec_close(audioCC);
	return flag;
}
