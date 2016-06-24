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
	 * @brief Allocate with audio samples.
	 */
	BufferSingular(real* const* const channels, std::size_t nChannels,
	               std::size_t nSamples);


	virtual Type getType() override;

	Vector<real>* getAudioChannels();
	std::size_t nAudioChannels() const noexcept;
private:
	std::vector<Vector<real>> audio;
};

} // namespace pg


// Implementations
inline
pg::BufferSingular::BufferSingular(real* const* const channels,
                                   std::size_t nChannels, std::size_t nSamples):
  audio(nChannels)
{
	for (std::size_t i = 0; i < nChannels; ++i)
	{
		audio[i] = Vector<real>(channels[i], nSamples);
	}
}
 
inline pg::Buffer::Type
pg::BufferSingular::getType()
{
	return Singular;
}

inline pg::Vector<pg::real>*
pg::BufferSingular::getAudioChannels()
{
	return this->audio.data();
}
inline std::size_t
pg::BufferSingular::nAudioChannels() const noexcept
{
	return audio.size();
}

#endif // !_POLYGAMMA_SINGULAR_BUFFERSINGULAR_HPP__
