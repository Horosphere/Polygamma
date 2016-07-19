#include "Axis.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QFont>
#include <QFontDatabase>

#include <QDebug>

namespace pg
{

inline QString defaultLabelingFunction(int val) noexcept
{
	return QString::number(val);
}


// Axis

Axis::Axis(QWidget* parent): QWidget(parent),
	orientation(Qt::TopEdge), invLogBase(0.0), nTicksMinor(3),
	penTick(Qt::black, 2), penTickMinor(Qt::black), penText(Qt::black),
	tickLength(12), tickLengthMinor(6), nSegments(0),
	labelingFunction(defaultLabelingFunction)
{
	setMinimumSize(1, 1);
}

inline void
Axis::setMarks(int r0, int r1, int n)
{
	// assert(r0 != r1);
	assert(n >= 0);

	raster0 = r0;
	raster1 = r1;
	nSegments = n;

	ticks.clear();
	ticksMinor.clear();

	double step = (raster1 - raster0) / (double) nSegments;
	double stepMinor = step / (nTicksMinor + 1);
	double tick = raster0; // Loop variable

	ticks.reserve(nSegments + 1);
	ticksMinor.reserve(nSegments * nTicksMinor);
	ticks.push_back(std::make_pair(tick, labelingFunction(0)));
	for (int i = 0; i < nSegments; ++i)
	{
		if (nTicksMinor)
		{
			if (invLogBase) // Logarithmic minor ticks
			{
				if (invLogBase > 0.0)
				{
					double dist = step;
					for (int i = 0; i < nTicksMinor; ++i)
					{
						dist *= invLogBase;
						ticksMinor.push_back((int)(dist + tick));
					}
				}
				else
				{
					double dist = step;
					for (int i = 0; i < nTicksMinor; ++i)
					{
						dist *= -invLogBase;
						ticksMinor.push_back((int)(step - dist + tick));
					}
				}
			}
			else // Linear minor ticks
			{
				double tickMinor = tick;
				for (int i = 0; i < nTicksMinor; ++i)
				{
					tickMinor += stepMinor;
					ticksMinor.push_back((int) tickMinor);
				}
			}
		}
		tick += step;
		ticks.push_back(std::make_pair((int) tick, labelingFunction(i + 1)));
	}
	Q_EMIT recalculationComplete();
	repaint();
}

void Axis::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setFont(font());

	int upper, lower;
	int upperMinor, lowerMinor;

	switch (orientation)
	{
	case Qt::TopEdge:
		upper = height() - tickLength;
		lower = height();
		upperMinor = height() - tickLengthMinor;
		lowerMinor = height();
		for (auto const& tick: ticks)
		{
			painter.setPen(penTick);
			painter.drawLine(tick.first, upper, tick.first, lower);
			painter.setPen(penText);
			painter.drawText(QPoint(tick.first, upper), tick.second);
		}
		painter.setPen(penTickMinor);
		for (auto const& tick: ticksMinor)
		{
			painter.drawLine(tick, upperMinor, tick, lowerMinor);
		}
		break;
	case Qt::BottomEdge:
		upper = 0;
		lower = tickLength;
		upperMinor = 0;
		lowerMinor = tickLengthMinor;
		for (auto const& tick: ticks)
		{
			painter.setPen(penTick);
			painter.drawLine(tick.first, upper, tick.first, lower);
			painter.setPen(penText);
			painter.drawText(QPoint(tick.first, upper), tick.second);
		}
		painter.setPen(penTickMinor);
		for (auto const& tick: ticksMinor)
		{
			painter.drawLine(tick, upperMinor, tick, lowerMinor);
		}
		break;
	case Qt::LeftEdge:
		upper = 0;
		lower = tickLength;
		upperMinor = 0;
		lowerMinor = tickLengthMinor;
		for (auto const& tick: ticks)
		{
			painter.setPen(penTick);
			painter.drawLine(upper, tick.first, lower, tick.first);
			painter.setPen(penText);
			painter.drawText(QPoint(upper, tick.first), tick.second);
		}
		painter.setPen(penTickMinor);
		for (auto const& tick: ticksMinor)
		{
			painter.drawLine(upperMinor, tick, lowerMinor, tick);
		}
		break;
	case Qt::RightEdge:
		upper = width() - tickLength;
		lower = width();
		upperMinor = width() - tickLengthMinor;
		lowerMinor = width();
		for (auto const& tick: ticks)
		{
			painter.setPen(penTick);
			painter.drawLine(upper, tick.first, lower, tick.first);
			painter.setPen(penText);
			painter.drawText(QPoint(upper, tick.first), tick.second);
		}
		painter.setPen(penTickMinor);
		for (auto const& tick: ticksMinor)
		{
			painter.drawLine(upperMinor, tick, lowerMinor, tick);
		}
		break;
	}
}

// AxisInterval
AxisInterval::AxisInterval(QWidget* parent): Axis(parent),
	interval(0, effectiveLength()),
	minTickWidth(150)
{
	setFixedHeight(30);
	moduli = {44100, 60, 60};
	moduli.insert(moduli.begin(), 1);
	setIntervalLabelingFunction([](long n, long m)
	{
		return QString::number(n * m);
	});
}

void AxisInterval::recalculate()
{
	int l = effectiveLength();

	long intervalLength = length(interval);

	modulus = 1;

	long baseLength = l;
	for (long const& m: moduli)
	{
		if (baseLength * m / intervalLength > minTickWidth) break;
		baseLength *= m;
		modulus *= m;
	}
	while (baseLength / intervalLength < minTickWidth)
	{
		baseLength *= 2;
		modulus *= 2;
	}

	// Floor and ceiling division
	tLow = interval.begin / modulus;
	long tHigh = 1 + ((interval.end - 1)) / modulus;
	if (tHigh - tLow > 50) return;
	setMarks((int)((tLow * modulus - interval.begin) * l / intervalLength),
	         (int)((tHigh * modulus - interval.begin) * l / intervalLength),
	         tHigh - tLow);
}

void AxisInterval::resizeEvent(QResizeEvent* event)
{
	recalculate();
	QWidget::resizeEvent(event);
}

} // namespace pg
