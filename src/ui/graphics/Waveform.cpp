#include "Waveform.hpp"

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
	maximise();
}

void pg::Waveform::paintEvent(QPaintEvent*)
{
	static QPen penWave(QColor(127,127,255),1);
	static QPen penCore(QColor(200,200,255),1);

	QPainter painter(this);
	painter.fillRect(rect(), Qt::black); // Background

	// Decides drawing mode. If the avaliable pixel per sample is less than
	// 1, then a lolipop diagram is drawn. Otherwise it is a standard waveform
	if (width() * UI_SAMPLE_DISPLAY_WIDTH * 64 < length(rangeX))
	{

		for (int x = 0; x < width(); ++x)
		{
			std::size_t sampleStart = (std::size_t) rasterToAxialX(x) /
					UI_SAMPLE_DISPLAY_WIDTH;
			std::size_t sampleEnd = (std::size_t) rasterToAxialX(x + 1) /
					UI_SAMPLE_DISPLAY_WIDTH;
			real minAbs = 0.0;
			real maxAbs = 0.0;
			real averageMin = 0.0;
			real averageMax = 0.0;
			for (std::size_t s = sampleStart; s < sampleEnd; ++s)
			{
				real sample = (*channel)[s];
				if (sample > 0.0)
				{
					maxAbs = std::max(sample, maxAbs);
					averageMax += maxAbs;
				}
				else
				{
					minAbs = std::min(sample, minAbs);
					averageMin += minAbs;
				}
			}
			real invSpan = 1.0 / (sampleEnd - sampleStart);
			averageMax *= invSpan;
			averageMin *= invSpan;
			painter.setPen(penWave);
			painter.drawLine(x, height() / 2 - (int)(maxAbs * 0.5 * height()),
						   x, height() / 2 - (int)(minAbs * 0.5 * height()));
			painter.setPen(penCore);
			painter.drawLine(x, height() / 2 - (int)(averageMax * 0.5 * height()),
						   x, height() / 2 - (int)(averageMin * 0.5 * height()));
		}

	}
	else
	{
		painter.setRenderHint(QPainter::Antialiasing);
		QPainter painterWave(this);
		painterWave.setPen(penWave);
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
