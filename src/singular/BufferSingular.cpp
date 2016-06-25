#include "BufferSingular.hpp"

#include <limits>
#include <typeinfo>
#include <cstring>
#include <iostream>

// Debugging purposes
#include <fstream>


extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <boost/timer/timer.hpp>

namespace pg
{

/**
 * Input ranges: (Casted linearly to [-1,1])
 * uint8_t: [0,255]
 * int16_t: [-32768,32767]
 * int32_t: [-2147483648,2147483647]
 * float: [-1.0f,1.0f]
 * double: [-1.0,1.0]
 * @brief dequantise converts a internal value used by FFMpeg to floating point.
 * @return A real number in the range [-1.0, 1.0].
 */
template<typename Value> real dequantise(Value);

template<typename Value, bool planar> bool
/** Template arguments:
 *     Value: The type of value stored in the audio stream. e.g. fltp -> float32
 *     planar: Is the audio planar or not.
 * @brief readAVInternal Internal method for reading an audio from a file. This
 *     function does not generate errors.
 * @param[out] channels Pre-allocated pointer to store the audio channels. THe
 *  channels themselves must not be allocated but will be allocated by the
 *  subroutine using std::malloc.
 * @param[out] nSamples Total number of samples.
 * @param formatContext Format context object that is pre-allocated and filled.
 * @param codecContext Codec context object that is pre-allocated and filled.
 * @param frame Frame object pre-allocated.
 * @param[in] stream The index of the audio stream in the file. Should be
 *     generated from av_find_best_stream with AVMEDIA_TYPE_AUDIO fed in.
 * @return true if succeeded. false if any subroutine allocation fails.
 */
readAudioStream(real** const channels, std::size_t* nSamples,
               AVFormatContext* const formatContext,
               AVCodecContext* const codecContext,
               AVFrame* const frame, int stream);

} // namespace pg


// Implementations


template<typename Value> inline pg::real pg::dequantise(Value value)
{
	// Unsigned integer
	if (typeid(Value) == typeid(uint8_t))
		// Toggle a single bit to achieve the linear map [0,255] -> [-128,127]
		// Example: 00000000_2 = 0_10 -> 10000000_2 = -128_10
		//          11111111_2 = 255_10 -> 01111111_2 = 127_10
		// The (uint8_t) in front of value is necessary but does not impact the
		// code itself.
		return ((int8_t)((uint8_t)value ^ 1 << 7)) / 128.0;
	if (typeid(Value) == typeid(uint16_t))
		return ((int16_t)((uint16_t)value ^ 1 << 15)) / 32768.0;
	if (typeid(Value) == typeid(uint32_t))
		return ((int32_t)((uint32_t)value ^ 1 << 31)) / 2147483648.0;

	// Signed integer
	if (typeid(Value) == typeid(int8_t))
		return value / 128.0;
	if (typeid(Value) == typeid(int16_t))
		return value / 32768.0;
	if (typeid(Value) == typeid(int32_t))
		return value / 2147483648.0;

	// Floating point types
	return (real) value;
}

template<typename Value, bool planar> inline bool
pg::readAudioStream(real** const channels, std::size_t* nSamples,
                 AVFormatContext* const formatContext,
               AVCodecContext* const codecContext,
               AVFrame* const frame, int stream)
{
	AVPacket readingPacket;
	av_init_packet(&readingPacket);

	// Load the audio
	for (int i = 0; i < codecContext->channels; ++i)
		channels[i] = nullptr;
	*nSamples = 0;

	// Use a loop to extract audio information.
	while (!av_read_frame(formatContext, &readingPacket))
	{
		if (readingPacket.stream_index == stream)
		{
			AVPacket decodingPacket = readingPacket;
			while (decodingPacket.size > 0)
			{
				// Decode the packet into frame(s)
				// The while loop ensures that all frames in the packet are
				// produced
				int gotFrame = 0;
				int dataSize = avcodec_decode_audio4(codecContext, frame,
				                                     &gotFrame, &decodingPacket);
				if (dataSize >= 0 && gotFrame)
				{
					decodingPacket.size -= dataSize;
					decodingPacket.data += dataSize;

					std::size_t chunkSamples = (std::size_t) frame->nb_samples;

					// Expand memory blocks
					for (int channel = 0; channel < frame->channels; ++channel)
					{
						real* temp = (real*) std::realloc(channels[channel],
						                                  (*nSamples + chunkSamples) * sizeof(real));
						if (temp) channels[channel] = temp;
						else // Unable to allocate more memory, terminates
						{
							for (int j = 0; j < frame->channels; ++j)
								std::free(channels[channel]);
							av_free_packet(&readingPacket);
							return false;
						}
					}
					// The frame (AVFrame* frame) has been fully decoded
					// The rest of the frame is decoded in the second while loop
					// below.
					if (planar)
					{
						for (int channel = 0; channel < frame->channels; ++channel)
						{
							Value* samples = (Value*) frame->extended_data[channel];
							for (int i = 0; i < frame->nb_samples; ++i)
							{
								channels[channel][*nSamples + i] = dequantise(samples[i]);
							}
						}
					}
					else
					{
						Value* samples = (Value*) frame->extended_data[0];
						// The following piece of code is duplicated 4 times.
						for (int channel = 0; channel < frame->channels; ++channel)
							for (std::size_t i = 0; i < chunkSamples; ++i)
							{
								channels[channel][*nSamples+ i]
								  = dequantise(samples[i * frame->channels + channel]);
							}
					}
					*nSamples += (std::size_t) frame->nb_samples;
				}
				else
				{
					decodingPacket.size = 0;
					decodingPacket.data = nullptr;
				}
			}
		}
		av_free_packet(&readingPacket); // Called to avoid memory leaks.
	}
	// Some codecs buffer frames while decoding
	// Flushes the buffer
	if (codecContext->codec->capabilities & CODEC_CAP_DELAY)
	{
		av_init_packet(&readingPacket);
		int gotFrame = 0;
		int dataSize;
		while ((dataSize = avcodec_decode_audio4(codecContext, frame,
		                   &gotFrame, &readingPacket) >= 0) && gotFrame)
		{
			// The frame has been fully decoded.
			std::size_t chunkSamples = (std::size_t) frame->nb_samples;

			// Expand memory blocks
			for (int channel = 0; channel < frame->channels; ++channel)
			{
				real* temp = (real*) std::realloc(channels[channel],
				                                  (*nSamples + chunkSamples) * sizeof(real));
				if (temp) channels[channel] = temp;
				else // Unable to allocate more memory, terminates
				{
					for (int j = 0; j < frame->channels; ++j)
						std::free(channels[channel]);
					av_free_packet(&readingPacket);
					return false;
				}
			}
			// The frame (AVFrame* frame) has been fully decoded
			// The rest of the frame is decoded in the second while loop
			// below.
			if (planar)
			{
				for (int channel = 0; channel < frame->channels; ++channel)
				{
					Value* samples = (Value*) frame->extended_data[channel];
					for (int i = 0; i < frame->nb_samples; ++i)
					{
						channels[channel][*nSamples + i] = dequantise(samples[i]);
					}
				}
			}
			else
			{
				Value* samples = (Value*) frame->extended_data[0];
				for (int channel = 0; channel < frame->channels; ++channel)
					for (std::size_t i = 0; i < chunkSamples; ++i)
					{
						channels[channel][*nSamples + i]
						  = dequantise(samples[i * frame->channels + channel]);
					}
			}
			*nSamples += (std::size_t) frame->nb_samples;
		}
	}
	return true;
}
pg::BufferSingular* pg::BufferSingular::fromFile(std::string fileName,
    std::string* error)
{
	boost::timer::auto_cpu_timer timer;
	AVFormatContext* formatContext = nullptr;
	if (avformat_open_input(&formatContext, fileName.c_str(), nullptr, nullptr))
	{
		*error = std::string("Unable to open ") + fileName;
		return nullptr;
	}
	if (avformat_find_stream_info(formatContext, nullptr) < 0)
	{
		avformat_close_input(&formatContext);
		*error = std::string("Unable to find stream info");
		return nullptr;
	}

	// Finding audio channels
	// TODO: After video becomes supported, no error will be produced if the file
	// has no audio stream.
	AVCodec* codec = nullptr;
	int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO,
										  -1, -1, &codec, 0);
	if (streamIndex < 0)
	{
		avformat_close_input(&formatContext);
		*error = std::string("The file does not have any audio stream");
		return nullptr;
	}
	AVStream* audioStream = formatContext->streams[streamIndex];
	AVCodecContext* codecContext = audioStream->codec;
	codecContext->codec = codec;
	if (avcodec_open2(codecContext, codecContext->codec, nullptr))
	{
		avformat_close_input(&formatContext);
		*error = std::string("Unable to open codec");
		return nullptr;
	}
	std::cout << "[IO]Found audio stream with the following information:"
			  << std::endl;
	std::cout << "[IO]Channels: " << codecContext->channels
			  << ", Sample rate: " << codecContext->sample_rate
			  << "/s, Format: " << av_get_sample_fmt_name(codecContext->sample_fmt)
			  << ", Bytes/Sample: " << av_get_bytes_per_sample(codecContext->sample_fmt)
			  << ", Duration: " << formatContext->duration / AV_TIME_BASE
			  << " s" << std::endl;

	AVFrame* frame = av_frame_alloc();
	if (!frame)
	{
		avcodec_close(codecContext);
		avformat_close_input(&formatContext);
		*error = "Unable to allocate frame";
		return nullptr;
	}
	real** channels = new real*[codecContext->channels];
	std::size_t nSamples = 0;
	bool flag;
	switch (codecContext->sample_fmt)
	{
	case AV_SAMPLE_FMT_U8:
		flag = readAudioStream<uint8_t, false>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S16:
		flag = readAudioStream<int16_t, false>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S32:
		flag = readAudioStream<int32_t, false>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_FLT:
		flag = readAudioStream<float, false>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_DBL:
		flag = readAudioStream<double, false>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_U8P:
		flag = readAudioStream<uint8_t, true>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S16P:
		flag = readAudioStream<int16_t, true>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S32P:
		flag = readAudioStream<int32_t, true>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_FLTP:
		flag = readAudioStream<float, true>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_DBLP:
		flag = readAudioStream<double, true>(channels, &nSamples,
				formatContext, codecContext, frame, audioStream->index);
		break;
	default: // Should not happen
		*error = std::string("Unrecognised sample format");
		return nullptr;
	}

	av_free(frame);
	avcodec_close(codecContext);
	avformat_close_input(&formatContext);

	if (!flag)
	{
		*error = std::string("Unable to allocate more memory");
		delete[] channels;
		return nullptr;
	}
	std::ofstream output;
	output.open("Channel0.txt");
	for (std::size_t j = 0; j < 44100; ++j)
	{
		output << channels[0][j] << std::endl;
	}
	output.close();

	BufferSingular* buffer = new BufferSingular(codecContext->channels);
	buffer->sampleRate = codecContext->sample_rate;
	// Move channels into the buffer
	for (std::size_t i = 0; i < (std::size_t)codecContext->channels; ++i)
	{
		/*
		real* channel = new real[nSamples];
		std::memcpy(channel, channels[i], nSamples * sizeof(real));
		std::free(channels[i]); // Must match readAudioStream's allocation pattern
		*/
		// Cannot use push_back since that prevents the first audio channel from
		// being read
		buffer->audio[i] = Vector<real>(nSamples, channels[i]);
	}
	delete[] channels;


	return buffer;
}
