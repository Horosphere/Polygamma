#ifndef _POLYGAMMA_MEDIA_PLAYBACK_H__
#define _POLYGAMMA_MEDIA_PLAYBACK_H__

#include <stdint.h>
#include <stdbool.h>

#include "media.h"

bool media_open(struct Media* const);
bool media_play(struct Media* const);
void media_close(struct Media* const);

#endif // !_POLYGAMMA_MEDIA_PLAYBACK_H__
