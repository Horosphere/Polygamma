#include "PanelMultimedia.hpp"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

extern "C"
{
#include "../../media/playback.h"
}
#include "../../singular/BufferSingular.hpp"

namespace pg
{

PanelMultimedia::PanelMultimedia(QWidget* parent): Panel(parent)
{
	setWindowTitle(tr("Video"));

	QVBoxLayout* layoutMain = new QVBoxLayout;
	QWidget* centralWidget = new QWidget;
	setWidget(centralWidget);
	centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	centralWidget->setLayout(layoutMain);
}
PanelMultimedia::~PanelMultimedia() noexcept
{
	for (auto& cache: caches)
	{
		if (!cache.second.cacheFile.empty())
			std::remove(cache.second.cacheFile.c_str());
	}
}

void PanelMultimedia::play(Buffer const* const buffer)
{
	assert(buffer);
	PlaybackCache* cache = &caches[buffer];
	if (cache->dirty != INTERVALINDEX_NULL)
		updateCache(buffer);
	if (auto const* const b = dynamic_cast<BufferSingular const* const>(buffer))
	{
		double const** samples = new double const*[b->nAudioChannels()];
		for (size_t i = 0; i < b->nAudioChannels(); ++i)
		{
			samples[i] = b->audioChannel(i)->getData();
		}
		struct Media* media = new Media;
		media->samples = (uint8_t const**) samples;
		media->sampleFormat = BufferSingular::SAMPLE_FORMAT;
		media->channelLayout = b->getChannelLayout();
		media->nChannels = b->nAudioChannels();
		media->sampleRate = b->timeBase();
		media->nSamples = b->duration();
		media->cursor = 0;
		audio_play(media);
	}
}

void PanelMultimedia::updateCache(Buffer const* const buffer) noexcept
{
	// Currently doesn't need to have a cache
	assert(caches.find(buffer) != caches.end());
	PlaybackCache* cache = &caches[buffer];

	if (isEmpty(cache->dirty)) return; // No update needs to be done
	return;
}

} // namespace pg
