#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "../core/polygamma.hpp"

namespace pg
{

/**
 * @brief The Audio struct is the struct for storing a multi-channel audio file.
 */
struct Audio
{
	struct Channel
	{
		Channel(): data(nullptr) {}
		/**
		 * @brief Channel Class for a single audio channel.
		 * @param nSamples Number of samples in data.
		 * @param data A pointer allocated using malloc/realloc with size
		 *     sizeof(double) * nSamples.
		 */
		Channel(std::size_t nSamples, real* const data):
			nSamples(nSamples), data(data)
		{
		}
		Channel(Channel&& channel): nSamples(channel.nSamples), data(channel.data)
		{
			channel.data = nullptr;
		}
		~Channel()
		{
			// The data is allocated using malloc and realloc
			std::free(data);
		}

		inline Channel& operator=(Channel&& channel)
		{
			this->nSamples = channel.nSamples;
			this->data = channel.data;
			channel.data = nullptr;
			return *this;
		}

		inline real& operator[](std::size_t index) noexcept
		{
			return data[index];
		}
		inline real operator[](std::size_t index) const noexcept
		{
			return data[index];
		}

		inline std::size_t getNSamples() const noexcept
		{
			return nSamples;
		}
		inline real* getData() const noexcept
		{
			return data;
		}

	private:
		std::size_t nSamples;
		/**
		 * @warning Each element must have length AUDIO_CHUNK_SIZE
		 * @brief data The signal
		 */
		real* data;
	};

	Audio(): nChannels(0), data(nullptr) {}
	Audio(std::size_t nChannels): nChannels(nChannels),
		data(new Channel[nChannels]) {}
	Audio(Audio&& audio): sampleRate(audio.sampleRate),
		nChannels(audio.nChannels), data(audio.data)
	{
		audio.data = nullptr;
	}

	~Audio() { delete[] data; }

	inline Audio& operator=(Audio&& audio)
	{
		sampleRate = audio.sampleRate;
		nChannels = audio.nChannels;
		data = audio.data;
		audio.data = nullptr;
		return *this;
	}

	inline std::size_t getNChannels() { return nChannels; }

	inline Channel& operator[](std::size_t index) { return data[index]; }
	inline Channel const& operator[](std::size_t index) const
	{ return data[index]; }

	std::size_t sampleRate;
private:
	std::size_t nChannels;
	Channel* data;
};

} // namespace pg
#endif // AUDIO_HPP
