#include "audio.hpp"

#include <iostream>

void pg::silence(BufferSingular* buffer)
{
	bool flag = false;
	for (std::size_t i = 0; i < buffer->nAudioChannels(); ++i)
	{
		auto selection = buffer->getSelection(i);
		flag = flag || !isEmpty(selection);
		pg::Vector<pg::real>* const channel = buffer->getAudioChannel(i);
		for (std::size_t j = selection.first; j < selection.second; ++j)
		{
			(*channel)[j] = 0.0;
		}
	}
	if (flag) buffer->notifyUpdate();
}
