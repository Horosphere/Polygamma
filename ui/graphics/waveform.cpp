#include "waveform.hpp"

#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

#include "../ui.hpp"

pg::Waveform::Waveform(QWidget* parent): Viewport2(parent),
	channel(nullptr)
{
	setDragging(true, false);
	setZoomFac(1.1, 1.0);

}
void pg::Waveform::setChannel(Audio::Channel const* const channel)
{
	this->channel = channel;
	setMaximumRange(0, ((int64_t) channel->getNSamples() - 1) * UI_SAMPLE_DISPLAY_WIDTH + 1,
					0, height());
}

void pg::Waveform::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black); // Background
	QRect square = rect();
	square.setLeft(axialToRasterX(44100));
	square.setTop(20);
	square.setBottom(height() - 20);
	square.setRight(axialToRasterX(88200));
	painter.fillRect(square, Qt::red);

	// Decides drawing mode. If the avaliable pixel per sample is less than
	// 1, then a lolipop diagram is drawn. Otherwise it is a standard waveform
	if (width() < length(rangeX) * UI_SAMPLE_DISPLAY_WIDTH)
	{
		QPainter outer(this);
		outer.setPen(QColor(0,127,255));
		QPainter inner(this);
		inner.setPen(QColor(255,255,255));

		for (int x = 0; x < width(); ++x)
		{
			std::size_t sampleStart = (std::size_t) rasterToAxialX(x) /
					UI_SAMPLE_DISPLAY_WIDTH;
			std::size_t sampleEnd = (std::size_t) rasterToAxialX(x + 1) /
					UI_SAMPLE_DISPLAY_WIDTH;
			real maxAbs = 0.0;
			real averageAbs = 0.0;
			for (std::size_t s = sampleStart; s < sampleEnd; ++s)
			{
				real abs = std::abs((*channel)[s]);
				maxAbs = std::max(maxAbs, abs);
				averageAbs += abs;
			}
			averageAbs /= (real) (sampleEnd - sampleStart);
			int lengthM = (int)(maxAbs * 0.5 * height());
			int lengthA = (int)(averageAbs * 0.5 * height());
			outer.drawLine(x, height() / 2 - lengthM,
						   x, height() / 2 + lengthM);
			inner.drawLine(x, height() / 2 - lengthA,
						   x, height() / 2 + lengthA);
		}

	}
	else
	{
		painter.setRenderHint(QPainter::Antialiasing);
		QPainter painterWave(this);
		painterWave.setPen(QColor(0,127,255));
		std::size_t sampleStart = (std::size_t) rasterToAxialX(0) /
				UI_SAMPLE_DISPLAY_WIDTH;
		std::size_t sampleEnd = (std::size_t) rasterToAxialX(width()) /
				UI_SAMPLE_DISPLAY_WIDTH;

		int thisSampleX;
		int thisSampleY;
		int nextSampleX = axialToRasterX(sampleStart * UI_SAMPLE_DISPLAY_WIDTH);
		int nextSampleY = (int)((1.0 - (*channel)[sampleStart]) * 0.5 * height());

		for (std::size_t i = sampleStart; i < sampleEnd; ++i)
		{
			thisSampleX = nextSampleX;
			thisSampleY = nextSampleY;
			nextSampleX = axialToRasterX((i + 1) * UI_SAMPLE_DISPLAY_WIDTH);
			nextSampleY = (int)((1.0 - (*channel)[i + 1]) * 0.5 * height());

			painterWave.drawLine(thisSampleX, thisSampleY,
								 nextSampleX, nextSampleY);
		}
	}

}
