#include "audio.hpp"

#include <iostream>

void pg::silence(BufferSingular* buffer)
{
	IntervalIndex changed(std::numeric_limits<std::size_t>::max(), 0);
	for (std::size_t i = 0; i < buffer->nAudioChannels(); ++i)
	{
		auto selection = buffer->getSelection(i);
		if (!isEmpty(selection))
		{
			changed += selection;
			pg::Vector<pg::real>* const channel = buffer->audioChannel(i);
			for (std::size_t j = selection.begin; j < selection.end; ++j)
			{
				(*channel)[j] = 0.0;
			}
		}
	}
	if (!isEmpty(changed))
		buffer->notifyUpdate(Buffer::Update::Data, changed);
}
