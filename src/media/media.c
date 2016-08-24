#include "media.h"

#include <assert.h>

void Media_init(struct Media* const media)
{
	assert(media);
	memset(media, 0, sizeof(struct Media));
}
void Media_set_cursor(struct Media* const m, size_t cursor)
{
	size_t bytes = av_get_bytes_per_sample(m->sampleFormat) * cursor;
	if (bytes < m->cursor)
	{
		size_t shift = m->cursor - bytes;
		for (size_t i = 0; i < m->nChannels; ++i)
			m->samples[i] -= shift;
	}
	else
	{
		size_t shift = bytes - m->cursor;
		for (size_t i = 0; i < m->nChannels; ++i)
			m->samples[i] += shift;
	}
}
size_t Media_get_cursor(struct Media* const m)
{
	return m->cursor / av_get_bytes_per_sample(m->sampleFormat);
}
