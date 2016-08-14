#include "audio.h"

#include <assert.h>

void Audio_init(struct Audio* const a)
{
	assert(a);
	memset(a, 0, sizeof(struct Audio));
}
