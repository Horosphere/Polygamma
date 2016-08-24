#ifndef _POLYGAMMA_MEDIA_IO_H__
#define _POLYGAMMA_MEDIA_IO_H__

#include "media.h"

/**
 * @brief Populates the samples and data in a struct Media from a file.
 * The sampleFormat field of media must be filled.
 * @return true if successful.
 */
bool Media_load_file(struct Media* const, char const* const fileName,
                     char const** const error);

bool Media_save_file(struct Media const* const,
                     char const* const fileName,
                     char const** const error);
#endif // !_POLYGAMMA_MEDIA_IO_H__
