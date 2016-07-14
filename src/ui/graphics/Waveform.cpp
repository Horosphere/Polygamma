#include "Waveform.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QFile>

#include <QDebug>

#include "../ui.hpp"

pg::Waveform::Waveform(BufferSingular const* const buffer,
                       std::size_t channelId,
                       QWidget* parent): Viewport2(parent),
	buffer(buffer), channelId(channelId),
	channel(buffer->audioChannel(channelId))
{
	setDragging(true, false);
	setZoomFac(1.1, 1.0);
	setSelecting(true, false);

	// In the past this was multiplied by UI_SAMPLE_DISPLAY_WIDTH to mimic
	// floating point
	setMaximumRange(0, ((long) channel->getSize() - 1) / UI_SAMPLE_DISPLAY_WIDTH,
	                0, height());
	maximise();
}

void pg::Waveform::paintEvent(QPaintEvent* event)
{
	Viewport2::paintEvent(event);
	QPainter painter(this);

	// Decides drawing mode. If the avaliable pixel per sample is less than
	// 1, then a lolipop diagram is drawn. Otherwise it is a standard waveform
	bool denseDrawing = width() * UI_SAMPLE_DISPLAY_WIDTH * 64 < length(rangeX);
	if (denseDrawing)
	{
		std::size_t step = 1 + length(rangeX) / width() / UI_SAMPLE_DISPLAY_WIDTH / 512;
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
			for (std::size_t s = sampleStart; s < sampleEnd; s += step)
			{
				real const sample = (*channel)[s];
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
			real invSpan = 1.0 / (sampleEnd - sampleStart) * step;
			averageMax *= invSpan;
			averageMin *= invSpan;
			painter.setPen(penEdge);
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
		painter.setPen(penCore);
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

			painter.drawLine(thisSampleX, thisSampleY,
			                 nextSampleX, nextSampleY);
		}
	}

	// Draw the selection
	IntervalIndex selection = buffer->getSelection(channelId);
	if (!isEmpty(selection))
	{
		painter.setCompositionMode(QPainter::CompositionMode_Difference);
		int begin = axialToRasterX(selection.begin);
		int end = axialToRasterX(selection.end);
		painter.fillRect(QRect(begin, 0, end - begin, height()), Qt::white);
	}
}
