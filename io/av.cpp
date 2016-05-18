#include "av.hpp"

#include <cstring>
#include <iostream>
#include <cstdio>
#include <vector>
#include <limits>
#include <typeinfo>
#include <chrono>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace pg
{

/**
 * Input ranges: (Casted linearly to [-1,1])
 * uint8_t: [0,255]
 * int16_t: [-32768,32767]
 * int32_t: [-2147483648,2147483647]
 * float: [-1.0f,1.0f]
 * double: [-1.0,1.0]
 * @brief dequantise converts a internal value used by ffmpeg to floating point.
 * @return A real number in the range [-1.0,1.0].
 */
template<typename Value> real dequantise(Value);

template<typename Value, bool planar> bool
/** Template arguments:
 *     Value: The type of value stored in the audio stream. e.g. fltp -> float32
 *     planar: Is the audio planar or not.
 * @brief readAVInternal Internal method for reading an audio from a file. This
 *     function does not generate errors.
 * @param formatContext Format context object pre-allocated and filled.
 * @param codecContext Codec context object pre-allocated and filled.
 * @param frame Frame object pre-allocated.
 * @param[in] stream The index of the audio stream in the file. Should be
 *     generated from av_find_best_stream with AVMEDIA_TYPE_AUDIO fed in.
 * @param[out] audio Its value will be filled by the audio information from the
 *     formatContext.
 * @return true if succeeded. false if allocation fails.
 */
readAVInternal(AVFormatContext* const formatContext,
			   AVCodecContext* const codecContext,
			   AVFrame* const frame, int stream, Audio* const audio);

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
pg::readAVInternal(AVFormatContext* const formatContext,
				   AVCodecContext* const codecContext,
				   AVFrame* const frame, int stream, Audio* const audio)
{
	// Monitors performance
	auto timeStamp = std::chrono::high_resolution_clock::now();

	// Temporary code for testing
	AVPacket readingPacket;
	av_init_packet(&readingPacket);


	// Load the audio
	real** channels = new real*[codecContext->channels];
	for (int i = 0; i < codecContext->channels; ++i)
		channels[i] = nullptr;
	std::size_t length = 0;


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
														  (length + chunkSamples) * sizeof(real));
						if (temp)
							channels[channel] = temp;
						else // Unable to allocate more memory, terminates
						{
							for (int j = 0; j < frame->channels; ++j)
								std::free(channels[channel]);
							av_free_packet(&readingPacket);
							delete[] channels;
							return false;
						}
					}
					// The frame (AVFrame* frame) has been fully decoded
					// The rest of the frame is decoded in the second while loop
					// below.
					if (planar)
					{
						Value* samples = (Value*) frame->data[0];
						// The following piece of code is duplicated 4 times.
						for (int channel = 0; channel < frame->channels; ++channel)
							for (std::size_t i = 0; i < chunkSamples; ++i)
							{
								channels[channel][length + i]
									= dequantise(samples[i * frame->channels + channel]);
							}
					}
					else
					{
						for (int channel = 0; channel < frame->channels; ++channel)
						{
							Value* samples = (Value*) frame->data[channel];
							for (int i = 0; i < frame->nb_samples; ++i)
							{
								channels[channel][length + i]
									= dequantise(samples[i]);
							}
						}
					}
					length += (std::size_t) frame->nb_samples;
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
												  (length + chunkSamples) * sizeof(real));
				if (temp)
					channels[channel] = temp;
				else // Unable to allocate more memory, terminates
				{
					for (int j = 0; j < frame->channels; ++j)
						std::free(channels[channel]);
					av_free_packet(&readingPacket);
					delete[] channels;
					return false;
				}
			}
			// The frame (AVFrame* frame) has been fully decoded
			// The rest of the frame is decoded in the second while loop
			// below.
			if (planar)
			{
				Value* samples = (Value*) frame->data[0];
				// The following piece of code is duplicated 4 times.
				for (int channel = 0; channel < frame->channels; ++channel)
					for (std::size_t i = 0; i < chunkSamples; ++i)
					{
						channels[channel][length + i]
							= dequantise(samples[i * frame->channels + channel]);
					}
			}
			else
			{
				for (int channel = 0; channel < frame->channels; ++channel)
				{
					Value* samples = (Value*) frame->data[channel];
					for (int i = 0; i < frame->nb_samples; ++i)
					{
						channels[channel][length + i] = dequantise(samples[i]);
					}
				}
			}
			length += (std::size_t) frame->nb_samples;
		}
	}
	// Load the audio into an instance of Audio class.
	for (std::size_t i = 0; i < (std::size_t) frame->channels; ++i)
	{
		(*audio)[i] = Audio::Channel(length, channels[i]);
	}

	// Monitors performance
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
					- timeStamp);
	std::cout << "[IO]Loading multimedia took "
			  << duration.count() << " ms" << std::endl;

	// Clean up
	delete[] channels;
	return true;
}

// This function is exposed to the rest of the application.
bool pg::readAV(char const fileName[], Audio* const audio,
				std::string* const error)
{
	AVFormatContext* formatContext = nullptr;
	if (avformat_open_input(&formatContext, fileName, nullptr, nullptr))
	{
		*error = std::string("Unable to open ") + fileName;
		return false;
	}
	if (avformat_find_stream_info(formatContext, nullptr) < 0)
	{
		avformat_close_input(&formatContext);
		*error = std::string("Unable to find stream info");
		return false;
	}

	// Finding audio channels
	AVCodec* codec = nullptr;
	int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO,
										  -1, -1, &codec, 0);
	if (streamIndex < 0)
	{
		avformat_close_input(&formatContext);
		*error = std::string("The file does not have any audio stream");
		return false;
	}
	AVStream* audioStream = formatContext->streams[streamIndex];
	AVCodecContext* codecContext = audioStream->codec;
	codecContext->codec = codec;
	if (avcodec_open2(codecContext, codecContext->codec, nullptr))
	{
		avformat_close_input(&formatContext);
		*error = std::string("Could not open the codec");
		return false;
	}
	std::cout << "[IO]Found audio stream with the following information:"
			  << std::endl;
	std::cout << "[IO]Channels: " << codecContext->channels
			  << ", Sample rate: " << codecContext->sample_rate
			  << "/s, Format: " << av_get_sample_fmt_name(codecContext->sample_fmt)
			  << ", Bytes/Sample: " << av_get_bytes_per_sample(codecContext->sample_fmt)
			  << ", Duration: " << formatContext->duration / AV_TIME_BASE
			  << " s" << std::endl;
	*audio = Audio(codecContext->channels);
	audio->sampleRate = (std::size_t) codecContext->sample_rate;

	AVFrame* frame = av_frame_alloc();
	if (!frame)
	{
		avcodec_close(codecContext);
		avformat_close_input(&formatContext);
		*error = "Unable to allocate frame";
		return false;
	}

	bool flag;
	// Using templates and switch to force the compiler to optimise this.
	switch (codecContext->sample_fmt)
	{
	case AV_SAMPLE_FMT_U8:
		flag = readAVInternal<uint8_t, false>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_S16:
		flag = readAVInternal<int16_t, false>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_S32:
		flag = readAVInternal<int32_t, false>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_FLT:
		flag = readAVInternal<float, false>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_DBL:
		flag = readAVInternal<double, false>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_U8P:
		flag = readAVInternal<uint8_t, true>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_S16P:
		flag = readAVInternal<int16_t, true>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_S32P:
		flag = readAVInternal<int32_t, true>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_FLTP:
		flag = readAVInternal<float, true>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	case AV_SAMPLE_FMT_DBLP:
		flag = readAVInternal<double, true>(formatContext, codecContext, frame, audioStream->index, audio);
		break;
	default: // Should not happen
		*error = std::string("Unrecognised sample format");
		return false;
	}

	av_free(frame);
	avcodec_close(codecContext);
	avformat_close_input(&formatContext);
	if (!flag)
	{
		*error = std::string("Unable to allocate more memory");
	}
	return flag;
}
