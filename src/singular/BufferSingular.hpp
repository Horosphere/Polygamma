#ifndef _POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__
#define _POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__

#include <vector>

#include "../core/polygamma.hpp"
#include "../core/Buffer.hpp"
#include "../math/Vector.hpp"

namespace pg
{

class BufferSingular final: public Buffer
{
public:
	/**
	 * This factory method is NOT directly exposed to Python, as it is required
	 * (in Python) to throw exceptions upon failure.
	 *
	 * @brief Reads a BufferSingular from the specified file.
	 * @param[in] fileName The path to the file.
	 * @param[out] error The error message. The space must be pre-allocated. An
	 *  error message of "" indicates no error.
	 * @return A BufferSingular object if the construction is successiful.
	 *  nullptr otherwise.
	 */
	static BufferSingular* fromFile(std::string fileName, std::string* error);


	virtual Type getType() override;

	std::size_t nAudioChannels() const noexcept;
	Vector<real>* getAudioChannel(std::size_t) noexcept;
	Vector<real> const* getAudioChannel(std::size_t) const noexcept;

	std::size_t sampleRate;
private:
	BufferSingular();
	BufferSingular(std::size_t nAudioChannels);

	std::vector<Vector<real>> audio;
};

} // namespace pg


// Implementations
inline
pg::BufferSingular::BufferSingular()
{
}
inline
pg::BufferSingular::BufferSingular(std::size_t nAudioChannels):
	audio(nAudioChannels)
{
}

inline pg::Buffer::Type
pg::BufferSingular::getType()
{
	return Singular;
}

inline std::size_t
pg::BufferSingular::nAudioChannels() const noexcept
{
	return audio.size();
}

inline pg::Vector<pg::real>*
pg::BufferSingular::getAudioChannel(std::size_t index) noexcept
{
	return &audio[index];
}
inline pg::Vector<pg::real> const*
pg::BufferSingular::getAudioChannel(std::size_t index) const noexcept
{
	return &audio[index];
}

#endif // !_POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__
