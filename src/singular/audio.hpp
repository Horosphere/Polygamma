#ifndef _POLYGAMMA_SINGULAR_AUDIO_HPP__
#define _POLYGAMMA_SINGULAR_AUDIO_HPP__

#include "BufferSingular.hpp"

namespace pg
{

/**
 * Exposed to Python
 * @brief Silences the buffer according to the selection in the buffer.
 */
void silence(BufferSingular*);

}

#endif // !_POLYGAMMA_SINGULAR_AUDIO_HPP__
