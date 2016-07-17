#include "BufferSingular.hpp"

#include <cstring>
#include <iostream>
#include <limits>
#include <typeinfo>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <boost/timer/timer.hpp>

#include "../core/text.hpp"

namespace pg
{

/**
 * Input ranges: (Casted linearly to [-1,1])
 * uint8_t: [0,255]
 * int16_t: [-32768,32767]
 * int32_t: [-2147483648,2147483647]
 * float: [-1.0f,1.0f]
 * double: [-1.0,1.0]
 * @brief dequantise converts a internal value used by FFMpeg to floating
 *  point.
 * @return A real number in the range [-1.0, 1.0].
 */
template<typename Value> real dequantise(Value);
/**
 * Output ranges: (Casted linearly from [-1,1])
 * uint8_t: [0,255]
 * int16_t: [-32768,32767]
 * int32_t: [-2147483648,2147483647]
 * float: [-1.0f,1.0f]
 * double: [-1.0,1.0]
 * @brief Inverse operator of \ref dequantise. The parameter must lie within
 *  the range [-1, 1]
 */
template<typename Value> Value quantise(real);

/**
 * @tparam Value The type of value stored in the audio stream. e.g. fltp ->
 *  float32
 * @tparam planar is true if the audio is stored planar. Interleaved otherwise.
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
template<typename Value, bool planar> bool
readAudioStream(real** const channels, std::size_t* nSamples,
                AVFormatContext* const formatContext,
                AVCodecContext* const codecContext,
                AVFrame* const frame, int stream);

/**
 * @brief Write the channels to the file. This function is not responsible for
 *  any allocations
 * @param samples A pre-allocated region (frame size * number of channels) as
 *  buffer.
 */
template<typename Value, bool planar> bool
writeAudioStream(std::FILE* const file, std::vector<Vector<real>> const& channels,
                 AVCodecContext* const codecContext, AVFrame* const frame,
                 Value* const samples);

} // namespace pg


// Implementations


template<typename Value> inline pg::real pg::dequantise(Value value)
{
	// TODO: Use hexadecimal floating point literals
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

template<typename Value> inline Value pg::quantise(real value)
{
	// Unsigned integer
	if (typeid(Value) == typeid(uint8_t))
		return (uint8_t)(1 << 7 ^ (int8_t)(value * 128.0));
	if (typeid(Value) == typeid(uint16_t))
		return (uint16_t)(1 << 7 ^ (int16_t)(value * 32768.0));
	if (typeid(Value) == typeid(uint32_t))
		return (uint32_t)(1 << 7 ^ (int32_t)(value * 2147483648.0));

	// Signed integer
	if (typeid(Value) == typeid(int8_t))
		return (int8_t)(value * 128.0);
	if (typeid(Value) == typeid(int16_t))
		return (int16_t)(value * 32768.0);
	if (typeid(Value) == typeid(int32_t))
		return (int32_t)(value * 2147483648.0);

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
							av_packet_unref(&readingPacket);
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
		av_packet_unref(&readingPacket); // Called to avoid memory leaks.
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
					av_packet_unref(&readingPacket);
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
    std::string* const error) noexcept
{
	std::cout << "[IO] Reading BufferSingular from file " << fileName
	          << std::endl;

	boost::timer::auto_cpu_timer timer;
	AVFormatContext* formatContext = nullptr;
	if (avformat_open_input(&formatContext, fileName.c_str(), nullptr, nullptr))
	{
		*error = "Unable to open " + fileName;
		return nullptr;
	}
	if (avformat_find_stream_info(formatContext, nullptr) < 0)
	{
		avformat_close_input(&formatContext);
		*error = "Unable to find stream info";
		return nullptr;
	}

	// Finding audio channels
	// TODO: Add video support and allows reading of audioless media
	AVCodec* codec = nullptr;
	int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO,
	                                      -1, -1, &codec, 0);
	if (streamIndex < 0)
	{
		avformat_close_input(&formatContext);
		*error = "The file does not have any audio stream";
		return nullptr;
	}
	AVStream* audioStream = formatContext->streams[streamIndex];
	AVCodecContext* codecContext = audioStream->codec;
	codecContext->codec = codec;
	if (avcodec_open2(codecContext, codecContext->codec, nullptr))
	{
		avformat_close_input(&formatContext);
		*error = "Unable to open codec";
		return nullptr;
	}
	std::cout << "[IO] Found audio stream:"
	          << std::endl;
	std::cout << "[IO] Channels: " << codecContext->channels
	          << ", Sample rate: " << codecContext->sample_rate
	          << "/s, Format: " << av_get_sample_fmt_name(codecContext->sample_fmt)
	          << ", Bitrate: " << codecContext->bit_rate
	          << ", Duration: " << formatContext->duration / AV_TIME_BASE
	          << "s" << std::endl;

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
		flag = readAudioStream<uint8_t, false>(channels, &nSamples, formatContext,
		                                       codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S16:
		flag = readAudioStream<int16_t, false>(channels, &nSamples, formatContext,
		                                       codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S32:
		flag = readAudioStream<int32_t, false>(channels, &nSamples, formatContext,
		                                       codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_FLT:
		flag = readAudioStream<float, false>(channels, &nSamples, formatContext,
		                                     codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_DBL:
		flag = readAudioStream<double, false>(channels, &nSamples, formatContext,
		                                      codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_U8P:
		flag = readAudioStream<uint8_t, true>(channels, &nSamples, formatContext,
		                                      codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S16P:
		flag = readAudioStream<int16_t, true>(channels, &nSamples, formatContext,
		                                      codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_S32P:
		flag = readAudioStream<int32_t, true>(channels, &nSamples, formatContext,
		                                      codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_FLTP:
		flag = readAudioStream<float, true>(channels, &nSamples, formatContext,
		                                    codecContext, frame, audioStream->index);
		break;
	case AV_SAMPLE_FMT_DBLP:
		flag = readAudioStream<double, true>(channels, &nSamples, formatContext,
		                                     codecContext, frame, audioStream->index);
		break;
	default: // Should not happen
		*error = "Unrecognised sample format";
		return nullptr;
	}

	av_free(frame);
	avcodec_close(codecContext);
	avformat_close_input(&formatContext);

	if (!flag)
	{
		*error = "Unable to allocate more memory";
		delete[] channels;
		return nullptr;
	}

	BufferSingular* buffer = new BufferSingular(codecContext->channel_layout);
	buffer->sampleRate = codecContext->sample_rate;
	buffer->title = fileName;
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
pg::BufferSingular* pg::BufferSingular::create(ChannelLayout cl,
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
template<typename Value, bool planar> inline bool
pg::writeAudioStream(std::FILE* const file, std::vector<Vector<real>> const& channels,
                     AVCodecContext* const codecContext, AVFrame* const frame,
                     Value* const samples)
{
	// Encode audio into frame
	std::size_t nSamples = channels[0].getSize();
	std::size_t frameSize = codecContext->frame_size;
	std::size_t nFrames = (nSamples - 1) / frameSize;
	std::size_t index = 0;
	std::size_t nChannels = codecContext->channels;
	AVPacket packet;
	// Encode all except for the last frame. The last frame is encoded at the
	// end with zero paddings
	for (std::size_t iFrame = 0; iFrame < nFrames; ++iFrame)
	{
		av_init_packet(&packet);
		packet.data = nullptr; // Allocated by encoder later
		packet.size = 0;

		if (planar)
		{
			for (std::size_t c = 0; c < nChannels; ++c)
			{
				std::size_t indexBegin = c * frameSize;
				for (std::size_t i = 0; i < frameSize; ++i)
					samples[indexBegin + i] = quantise<Value>(channels[c][i + index]);
			}
		}
		else
		{
			for (std::size_t c = 0; c < nChannels; ++c)
				for (std::size_t i = 0; i < frameSize; ++i)
					samples[nChannels * i + c] = quantise<Value>(channels[c][i + index]);
		}

		int gotOutput;
		if (avcodec_encode_audio2(codecContext, &packet, frame, &gotOutput) < 0)
			return false;
		if (gotOutput)
		{
			std::fwrite(packet.data, 1, packet.size, file);
			av_packet_unref(&packet);
		}

		index += codecContext->frame_size;
	}

	// The last frame is encoded here
	std::size_t trailing = nSamples - index;
	av_init_packet(&packet);
	packet.data = nullptr; // Allocated by encoder later
	packet.size = 0;

	if (planar)
	{
		for (std::size_t c = 0; c < nChannels; ++c)
		{
			std::size_t indexBegin = c * frameSize;
			for (std::size_t i = 0; i < trailing; ++i)
				samples[indexBegin + i] = quantise<Value>(channels[c][i + index]);
			for (std::size_t i = trailing; i < frameSize; ++i)
				samples[indexBegin + i] = 0;
		}
	}
	else
	{
		for (std::size_t c = 0; c < nChannels; ++c)
		{
			for (std::size_t i = 0; i < trailing; ++i)
				samples[nChannels * i + c] = quantise<Value>(channels[c][i + index]);
			for (std::size_t i = trailing; i < frameSize; ++i)
				samples[nChannels * i + c] = 0;
		}
	}

	int gotOutput;
	if (avcodec_encode_audio2(codecContext, &packet, frame, &gotOutput) < 0)
		return false;
	if (gotOutput)
	{
		std::fwrite(packet.data, 1, packet.size, file);
		av_packet_unref(&packet);
	}
	return true;
}
bool pg::BufferSingular::saveToFile(std::string fileName,
                                    std::string* const error) noexcept
{
	boost::timer::auto_cpu_timer timer;

	std::cout << "[IO] Writing BufferSingular to file " << fileName
	          << std::endl;
	AVOutputFormat* format = av_guess_format(nullptr, fileName.c_str(), nullptr);
	if (!format)
	{
		*error = "Could not find suitable audio format";
		return false;
	}
	AVCodec* codec = avcodec_find_encoder(format->audio_codec);
	if (!codec)
	{
		*error = "Unable to find codec";
		return false;
	}

	AVCodecContext* codecContext = avcodec_alloc_context3(codec);
	if (!codecContext)
	{
		*error = "Unable to allocate codec context";
		return false;
	}

	codecContext->bit_rate = 128000; // TODO: Calculate this automatically

	// Use the last sample format
	AVSampleFormat const* sampleFormat = codec->sample_fmts;
	while (*sampleFormat != AV_SAMPLE_FMT_NONE)
		++sampleFormat;
	--sampleFormat;
	codecContext->sample_fmt = *sampleFormat;

	codecContext->sample_rate = this->sampleRate;
	codecContext->channel_layout = this->channelLayout;
	codecContext->channels = this->nAudioChannels();

	if (avcodec_open2(codecContext, codec, nullptr) < 0)
	{
		*error = "Could not open codec";
		avcodec_close(codecContext);
		return false;
	}

	int bufferSize = av_samples_get_buffer_size(nullptr, codecContext->channels,
	                 codecContext->frame_size, codecContext->sample_fmt, 0);

	if (bufferSize < 0)
	{
		*error = "Could not get sample buffer size";
		avcodec_close(codecContext);
		return false;
	}

	// Allocate frame
	AVFrame* frame = av_frame_alloc();
	if (!frame)
	{
		*error = "Could not allocate audio frame";
		avcodec_close(codecContext);
		return false;
	}

	frame->nb_samples = codecContext->frame_size;
	frame->format = codecContext->sample_fmt;
	frame->channel_layout = codecContext->channel_layout;

	// Open the file
	std::FILE* file = std::fopen(fileName.c_str(), "wb");
	if (!file)
	{
		*error = "Could not open file";
		av_free(frame);
		avcodec_close(codecContext);
		return false;
	}

	void* samples = av_malloc(bufferSize);
	if (!samples)
	{
		*error = "Unable to allocate samples";
		av_free(frame);
		avcodec_close(codecContext);
		return false;
	}
	if (avcodec_fill_audio_frame(frame, codecContext->channels, codecContext->sample_fmt,
	                             (uint8_t const*) samples, bufferSize, 0) < 0)
	{
		*error = "Unable to fill audio frame with samples";
		av_free(samples);
		av_free(frame);
		avcodec_close(codecContext);
		return false;
	}

	bool flag;
	switch (codecContext->sample_fmt)
	{
	case AV_SAMPLE_FMT_U8:
		flag = writeAudioStream<uint8_t, false>(file, audio, codecContext, frame,
		                                        (uint8_t* const) samples);
		break;
	case AV_SAMPLE_FMT_S16:
		flag = writeAudioStream<int16_t, false>(file, audio, codecContext, frame,
		                                        (int16_t* const) samples);
		break;
	case AV_SAMPLE_FMT_S32:
		flag = writeAudioStream<int32_t, false>(file, audio, codecContext, frame,
		                                        (int32_t* const) samples);
		break;
	case AV_SAMPLE_FMT_FLT:
		flag = writeAudioStream<float, false>(file, audio, codecContext, frame,
		                                      (float* const) samples);
		break;
	case AV_SAMPLE_FMT_DBL:
		flag = writeAudioStream<double, false>(file, audio, codecContext, frame,
		                                       (double* const) samples);
		break;
	case AV_SAMPLE_FMT_U8P:
		flag = writeAudioStream<uint8_t, true>(file, audio, codecContext, frame,
		                                       (uint8_t* const) samples);
		break;
	case AV_SAMPLE_FMT_S16P:
		flag = writeAudioStream<int16_t, true>(file, audio, codecContext, frame,
		                                       (int16_t* const) samples);
		break;
	case AV_SAMPLE_FMT_S32P:
		flag = writeAudioStream<int32_t, true>(file, audio, codecContext, frame,
		                                       (int32_t* const) samples);
		break;
	case AV_SAMPLE_FMT_FLTP:
		flag = writeAudioStream<float, true>(file, audio, codecContext, frame,
		                                     (float* const) samples);
		break;
	case AV_SAMPLE_FMT_DBLP:
		flag = writeAudioStream<double, true>(file, audio, codecContext, frame,
		                                      (double* const) samples);
		break;
	default:
		*error = "[IO] No valid sample format found";
		av_free(samples);
		std::fclose(file);
		av_free(frame);
		avcodec_close(codecContext);
		return false;
	}


	if (!flag)
		*error = "Error while encoding audio";
	av_free(samples);
	std::fclose(file);
	av_free(frame);
	avcodec_close(codecContext);
	return flag;
}
