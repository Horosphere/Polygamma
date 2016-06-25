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
	 *	error message of "" indicates no error.
	 * @return A BufferSingular object if the construction is successiful.
	 */
	static BufferSingular* fromFile(std::string fileName, std::string* error);
	              


	virtual Type getType() override;

	std::size_t nAudioChannels() const noexcept;

	std::vector<Vector<real>> audio;
	std::size_t sampleRate;
private:
	BufferSingular();
};

} // namespace pg


// Implementations
inline
pg::BufferSingular::BufferSingular()
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

#endif // !_POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__
