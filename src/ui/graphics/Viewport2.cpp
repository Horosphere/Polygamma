#include "Viewport2.hpp"

#include <cmath>

#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>

//#include <QDebug>

pg::Viewport2::Viewport2(QWidget* parent) : QWidget(parent),
	rangeX(0, width()), rangeY(0, height()),
	colourBG(Qt::black), penGrid(Qt::gray, 2), penGridMinor(Qt::gray, 1),
	dragFacX(0.0), dragFacY(0.0),
	zoomFacX(1.0), zoomFacY(1.0),
	isDraggable(false), isDraggableX(false), isDraggableY(false),
	isZoomable(false), isSelectibleX(false), isSelectibleY(false),
	axisX(nullptr), axisY(nullptr),

	maxRangeX(0, width()), maxRangeY(0, height()),
	dragging(false), rubberBand(nullptr)
{
	setMinimumSize(1, 1); // Prevent division by 0
}

void pg::Viewport2::mousePressEvent(QMouseEvent* event)
{
	dragPos = event->pos();
	if (event->buttons() & Qt::MidButton)
	{
		dragging = isDraggable;
	}
	else if (event->buttons() & Qt::LeftButton && rubberBand)
	{
		selectTopLeft = event->pos();
		if (!isSelectibleY)
			selectTopLeft.setY(rect().top());
		else if (!isSelectibleX)
			selectTopLeft.setX(rect().left());
		rubberBand->setGeometry(QRect(selectTopLeft, QSize()));
		rubberBand->show();
	}
	event->accept();
}
// This method shall handle panning
void pg::Viewport2::mouseMoveEvent(QMouseEvent* event)
{
	if (dragging)
	{
		if (isDraggableX) // Dragging on X enabled
		{
			rangeX = translate(rangeX, maxRangeX,
			                   (axial_coord)((dragPos.x() - event->pos().x()) * dragFacX));
		}
		if (isDraggableY) // Dragging on Y enabled
		{
			rangeY = translate(rangeY, maxRangeY,
			                   (axial_coord)((dragPos.y() - event->pos().y()) * dragFacY));
		}

		if (isDraggable)
		{
			dragPos = event->pos();
			event->accept();
			Q_EMIT rangeXChanged(rangeX);
			Q_EMIT rangeYChanged(rangeY);
		}
	}
	if (rubberBand)
	{
		QPoint pos = event->pos();
		if (!isSelectibleY) pos.setY(rect().bottom());
		else if (!isSelectibleX) pos.setX(rect().right());
		rubberBand->setGeometry(QRect(selectTopLeft, pos).normalized());
	}
	event->accept();
}
void pg::Viewport2::mouseReleaseEvent(QMouseEvent* event)
{
	dragging = false;
	if (rubberBand && rubberBand->isVisible())
	{
		rubberBand->hide();
		QRect selection = rubberBand->rect().normalized();
		if (selection.isValid()) // Must check
		{
			if (isSelectibleX)
			{
				Interval<axial_coord> iX(rasterToAxialX(rubberBand->x()),
				                     rasterToAxialX(rubberBand->x() + selection.right()));
				iX = clamp(iX, maxRangeX.begin, maxRangeX.end);
				if (isSelectibleY)
				{
					Interval<axial_coord> iY(rasterToAxialY(rubberBand->y()),
					                     rasterToAxialY(rubberBand->y() + selection.bottom()));
					iY = clamp(iY, maxRangeY.begin, maxRangeY.end);
					Q_EMIT selectionXY(iX, iY);
				}
				else
					Q_EMIT selectionX(iX);
			}
			else
			{
				Interval<axial_coord> iY(rasterToAxialY(rubberBand->y()),
				                     rasterToAxialY(rubberBand->y() + selection.bottom()));
				iY = clamp(iY, maxRangeY.begin, maxRangeY.end);
				Q_EMIT selectionY(iY);
			}
		}
		else
		{
			if (isSelectibleX)
			{
				if (isSelectibleY)
					Q_EMIT selectionXY(Interval<axial_coord>(0, 0), Interval<axial_coord>(0, 0));
				else
					Q_EMIT selectionX(Interval<axial_coord>(0, 0));
			}
			else
				Q_EMIT selectionY(Interval<axial_coord>(0, 0));
		
		}
	}
	event->accept();
}

// This method shall handle zooming
void pg::Viewport2::wheelEvent(QWheelEvent* event)
{
	double steps = event->delta() / -120.0;
	if (steps < 0.0)
	{
		if (zoomFacX != 1.0) // Zoom enabled
		{
			axial_coord axialX = rasterToAxialX(event->x());
			double fac = std::pow(zoomFacX, steps);
			if (length(rangeX) * fac > width())
			{
				rangeX = scale(rangeX, fac, axialX);
				dragFacX = length(rangeX) / (double) width();
			}
		}
		if (zoomFacY != 1.0)
		{
			axial_coord axialY = rasterToAxialY(event->y());
			double fac = std::pow(zoomFacY, steps);
			if (length(rangeX) * fac > height())
			{
				rangeY = scale(rangeY, fac, axialY);
				dragFacY = length(rangeY) / (double) height();
			}
		}
	}
	else
	{
		if (zoomFacX != 1.0) // Zoom enabled
		{
			double fac = std::pow(zoomFacX, steps);
			rangeX = scale(rangeX, maxRangeX, fac, rasterToAxialX(event->x()));
			dragFacX = length(rangeX) / (double)width();
		}
		if (zoomFacY != 1.0)
		{
			double fac = std::pow(zoomFacY, steps);
			rangeY = scale(rangeY, maxRangeY, fac, rasterToAxialY(event->y()));
			dragFacY = length(rangeY) / (double)height();
		}
	}
	if (isZoomable)
	{
		event->accept();
		Q_EMIT rangeXChanged(rangeX);
		Q_EMIT rangeYChanged(rangeY);
	}
}
void pg::Viewport2::resizeEvent(QResizeEvent* event)
{
	dragFacX = length(rangeX) / (double)width();
	dragFacY = length(rangeY) / (double)height();

	// Recalculate ranges
	if (event->oldSize().isValid())
	{
		axial_coord resultXLen = length(rangeX) * event->size().width()
		                     / event->oldSize().width();
		axial_coord resultYLen = length(rangeX) * event->size().height()
		                     / event->oldSize().height();

		if (resultXLen > length(maxRangeX))
			rangeX = maxRangeX;
		else rangeX.end = rangeX.begin + resultXLen;
		if (resultYLen > length(maxRangeY))
			rangeY = maxRangeY;
		else rangeY.end = rangeY.begin + resultYLen;
	}
	this->QWidget::resizeEvent(event);

}

void pg::Viewport2::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), colourBG);
	if (axisX)
	{
		painter.setPen(penGrid);
		for (auto const& tick: axisX->ticks)
			painter.drawLine(tick.first, 0, tick.first, height());
		painter.setPen(penGridMinor);
		for (auto const& tick: axisX->ticksMinor)
			painter.drawLine(tick, 0, tick, height());
	}
	if (axisY)
	{
		painter.setPen(penGrid);
		for (auto const& tick: axisY->ticks)
			painter.drawLine(0, tick.first, width(), tick.first);
		painter.setPen(penGridMinor);
		for (auto const& tick: axisY->ticksMinor)
			painter.drawLine(0, tick, width(), tick);
	}
}

void pg::Viewport2::updateFromAxes()
{
	AxisInterval* ai = nullptr;
	if ((ai = dynamic_cast<AxisInterval*>(axisX)))
	{
		rangeX = ai->interval;
	}
	if ((ai = dynamic_cast<AxisInterval*>(axisY)))
	{
		rangeY = ai->interval;
	}
	repaint();
}
