#ifndef _POLYGAMMA_MEDIA_PLAYBACK_H__
#define _POLYGAMMA_MEDIA_PLAYBACK_H__

#include <stdint.h>
#include <stdbool.h>

#include "audio.h"

bool audio_play(struct Audio* const);
void audio_stop(struct Audio* const);

#endif // !_POLYGAMMA_MEDIA_PLAYBACK_H__
