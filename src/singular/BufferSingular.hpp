#ifndef _POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__
#define _POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__

#include <vector>
#include <utility>

extern "C"
{
#include <libavutil/channel_layout.h>
}

#include "../core/python.hpp"
#include "../core/polygamma.hpp"
#include "../core/Buffer.hpp"
#include "../math/Vector.hpp"

namespace pg
{


class BufferSingular final: public Buffer
{
public:
	typedef std::pair<std::size_t, std::size_t> AudioInterval;

	/**
	 * This factory method is not directly exposed to Python, as it is required
	 * (in Python) to throw exceptions upon failure.
	 *
	 * @brief Reads a BufferSingular from the specified file.
	 * @param[in] fileName The path to the file.
	 * @param[out] error The error message. The space must be pre-allocated. An
	 *  error message of "" indicates no error.
	 * @return A BufferSingular object if the construction is successiful.
	 *  nullptr otherwise.
	 */
	static BufferSingular* fromFile(std::string fileName,
	                                std::string* const error) noexcept;
	/**
	 * This factory method is not directly exposed to Python, as it is required
	 * (in Python) to throw exceptions upon failure.
	 *
	 * @brief Creates a buffer singular from the parameters.
	 * @param[in] channelLayout See libavutil/channel_layout.h in ffmpeg's
	 *  documentation.
	 * @param[in] bitRate
	 * @param[in] sampleRate
	 * @param[out] error is filled when the creation fails.
	 * @return A BufferSingular object if the construction is successiful.
	 *  nullptr otherwise.
	 */
	static BufferSingular* create(ChannelLayout channelLayout,
	                              std::size_t sampleRate,
	                              std::size_t duration,
	                              std::string* const error) noexcept;


	virtual Type getType() const noexcept override;
	virtual bool saveToFile(std::string fileName,
	                        std::string* const error) override;

	/**
	 * Exposed to Python
	 */
	std::size_t nAudioChannels() const noexcept;
	/**
	 * Exposed to Python
	 */
	std::size_t nAudioSamples() const noexcept;

	Vector<real>* getAudioChannel(std::size_t);
	Vector<real> const* getAudioChannel(std::size_t) const;

	/**
	 * Exposed to Python
	 * @brief Sets a selection on every channel.
	 */
	void select(std::size_t begin, std::size_t end) throw(PythonException);
	/**
	 * Exposed to Python
	 * @brief Sets a selection on the channel given. Only one selection can exist
	 *  on a channel at a time.
	 */
	void select(std::size_t channel, std::size_t begin,
	            std::size_t end) throw(PythonException);
	/**
	 * Exposed to Python
	 * @brief Sets a selection on the channel given. Only one selection can exist
	 *  on a channel at a time.
	 */
	void select(std::size_t channel, AudioInterval selection) throw(PythonException);
	/**
	 * Exposed to Python
	 * Has the same effect as select(channel, std::make_pair(0, 0));
	 * @brief Remove the selection on the channel.
	 */
	void clearSelect();
	/**
	 * Exposed to Python
	 * Has the same effect as select(channel, std::make_pair(0, 0));
	 * @brief Remove the selection on the channel.
	 */
	void clearSelect(std::size_t channel) throw(PythonException);
	/**
	 * Exposed to Python
	 * @brief Obtain the selection on the given channel.
	 */
	AudioInterval getSelection(std::size_t channel) const throw(PythonException);

	std::size_t sampleRate;
private:
	BufferSingular();
	BufferSingular(ChannelLayout channelLayout);

	ChannelLayout channelLayout;

	// These two vectors must have the same length.
	std::vector<Vector<real>> audio;
	std::vector<AudioInterval> selections;
};

bool isEmpty(BufferSingular::AudioInterval const&) noexcept;

} // namespace pg


// Implementations

inline pg::BufferSingular::BufferSingular()
{
}
inline pg::BufferSingular::BufferSingular(ChannelLayout channelLayout):
	channelLayout(channelLayout),
	audio(av_get_channel_layout_nb_channels(channelLayout)),
	selections(audio.size())
{
	for (auto& selection: selections)
		selection.first = selection.second = 0;
}

inline pg::Buffer::Type
pg::BufferSingular::getType() const noexcept
{
	return Singular;
}

inline std::size_t
pg::BufferSingular::nAudioChannels() const noexcept
{
	return audio.size();
}
inline std::size_t
pg::BufferSingular::nAudioSamples() const noexcept
{
	return audio.empty() ? 0 : audio[0].getSize();
}

inline pg::Vector<pg::real>*
pg::BufferSingular::getAudioChannel(std::size_t index)
{
	return &audio[index];
}
inline pg::Vector<pg::real> const*
pg::BufferSingular::getAudioChannel(std::size_t index) const
{
	return &audio[index];
}
inline void
pg::BufferSingular::select(std::size_t begin, std::size_t end)
throw(PythonException)
{
	if (begin > end || end >= nAudioSamples())
		throw PythonException{"Sample index out of range", PythonException::IndexError};
	for (auto& selection: selections)
		selection = std::make_pair(begin, end);
	signalUpdate();
}
inline void
pg::BufferSingular::select(std::size_t channel,
                           std::size_t begin, std::size_t end) throw(PythonException)
{
	if (channel >= nAudioChannels())
		throw PythonException{"Channel index out of range", PythonException::IndexError};
	if (begin > end || end >= nAudioSamples())
		throw PythonException{"Sample index out of range", PythonException::IndexError};
	selections[channel] = std::make_pair(begin, end);
	signalUpdate();
}
inline void
pg::BufferSingular::select(std::size_t channel, AudioInterval selection)
throw(PythonException)
{
	if (channel >= nAudioChannels())
		throw PythonException{"Channel index out of range", PythonException::IndexError};
	if (selection.first > selection.second|| selection.second >= nAudioSamples())
		throw PythonException{"Sample index out of range", PythonException::IndexError};
	selections[channel] = selection;
	signalUpdate();
}
inline void
pg::BufferSingular::clearSelect()
{
	for (auto& selection: selections)
		selection = std::make_pair(0, 0);
	signalUpdate();
}
inline void
pg::BufferSingular::clearSelect(std::size_t channel) throw(PythonException)
{
	if (channel >= nAudioChannels())
		throw PythonException{"Channel index out of range", PythonException::IndexError};
	selections[channel] = std::make_pair(0, 0);
	signalUpdate();
}
inline pg::BufferSingular::AudioInterval
pg::BufferSingular::getSelection(std::size_t channel) const throw(PythonException)
{
	if (channel >= nAudioChannels())
		throw PythonException{"Channel index out of range", PythonException::IndexError};
	return selections[channel];
}
inline bool pg::isEmpty(BufferSingular::AudioInterval const& ai) noexcept
{
	return ai.first >= ai.second;
}
#endif // !_POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__
