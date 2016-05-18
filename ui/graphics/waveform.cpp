#include "waveform.hpp"

#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

pg::Waveform::Waveform(QWidget* parent): Viewport2(parent),
	channel(nullptr)
{
	setDragging(true, false);
	setZoomFac(1.1, 1.0);
	setMaximumRange(0.0, 1.0, -1.0, 1.0);
	setMinimumSpan(4.0, 0.0);


}
void pg::Waveform::setChannel(Audio::Channel const* const channel)
{
	this->channel = channel;
	rangeX = Interval<double>(0.0, channel->getNSamples());
}

void pg::Waveform::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black); // Background

	// Decides drawing mode. If the avaliable pixel per sample is less than
	// 1, then a lolipop diagram is drawn. Otherwise it is a standard waveform
	if (true || width() < length(rangeX))
	{
		for (int x = 0; x < width(); ++x)
		{
			std::size_t sampleId = (std::size_t) rasterToAxialX((double)x);
			if (sampleId >= channel->getNSamples())
				sampleId = channel->getNSamples() - 1;
			real sample = (*channel)[sampleId];
			int y = (int)((1.0 - sample) * height() / 2);
			painter.drawLine(QPoint(x, height() / 2), QPoint(x, y));
		}

	}
	else
	{
		painter.setRenderHint(QPainter::Antialiasing);

	}

}
