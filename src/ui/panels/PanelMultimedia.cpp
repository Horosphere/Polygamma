#include "PanelMultimedia.hpp"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

#include "../../singular/BufferSingular.hpp"

namespace pg
{


PanelMultimedia::PanelMultimedia(QWidget* parent): Panel(parent),
	videoOutput(new QtAV::VideoOutput), avPlayer(new QtAV::AVPlayer(this)),
	cacheIndex(0)
{
	setWindowTitle(tr("Video"));

	QVBoxLayout* layoutMain = new QVBoxLayout;
	QWidget* centralWidget = new QWidget;
	setWidget(centralWidget);
	centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	centralWidget->setLayout(layoutMain);

	if (videoOutput->widget())
	{
		avPlayer->setRenderer(videoOutput);
		layoutMain->addWidget(videoOutput->widget());
	}
	else
	{
		QMessageBox::warning(0, tr("QtAV error"),
		                     tr("Unable to create video renderer"));
		// Do not return here
	}


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
	if (!error.empty())
	{
		QMessageBox::warning(0, tr("Playback error"), tr(error.c_str()));
		return;
	}
	if (cache->cacheFile.empty())
	{
		return;
	}
	qDebug() << "[UI] Playback started";
	avPlayer->play(QString::fromStdString(cache->cacheFile));
}

void PanelMultimedia::updateCache(Buffer const* const buffer) noexcept
{
	assert(caches.find(buffer) != caches.end());
	PlaybackCache* cache = &caches[buffer];

	if (isEmpty(cache->dirty)) return; // No update needs to be done

	if (auto const* b = dynamic_cast<BufferSingular const*>(buffer))
	{
		cache->cacheFile = cachingDirectory + "/cache_" +
		                   std::to_string(cacheIndex) + ".avi";
		bool flag = b->exportToFile(cache->cacheFile, &error);
		if (flag)
			error.clear();
		else
		{
			cache->cacheFile = "";
			return;
		}
	}
	else
	{
		qDebug() << "[UI] Unrecognised Buffer Type";
		assert(false && "Buffer type unrecognised by PanelMultimedia");
	}

	cache->dirty = INTERVALINDEX_NULL;
}

} // namespace pg
