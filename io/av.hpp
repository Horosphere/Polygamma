#ifndef IO_H
#define IO_H

#include <string>

#include "../math/audio.hpp"

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
#endif // IO_H
