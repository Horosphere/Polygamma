#include "media.h"

#include <assert.h>

void Media_init(struct Media* const media)
{
	assert(media);
	memset(media, 0, sizeof(struct Media));
}
