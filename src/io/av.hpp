#ifndef _POLYGAMMA_IO_AV_HPP__
#define _POLYGAMMA_IO_AV_HPP__

#include <string>

#include "../math/Audio.hpp"

namespace pg
{
/**
 * @brief readFile Reads an video/audio file using FFmpeg
 * @param[in] fileName The path to the desired media file.
 * @param[out] audio The output.
 * @param[out] error A pointer used to accept errors. Must not be null.
 * @return true if successful. Otherwise false and *error is filled with the
 *         appropriate error message.
 */
bool readAV(char const fileName[], Audio* const audio, std::string* const error);

} // namespace pg


#endif // !_POLYGAMMA_IO_AV_HPP__

