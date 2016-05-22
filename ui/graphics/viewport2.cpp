#include "viewport2.hpp"

#include <cmath>

#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QGraphicsView>

#include <QDebug>

pg::Viewport2::Viewport2(QWidget *parent) : QWidget(parent),
	rangeX(0, width()), rangeY(0, height()),
	dragFacX(0.0), dragFacY(0.0),
	zoomFacX(1.0), zoomFacY(1.0),
	isDraggable(false), isDraggableX(false), isDraggableY(false),
	isZoomable(false),

	maxRangeX(0, width()), maxRangeY(0, height()),
	dragging(false)
{

}
pg::Viewport2::~Viewport2()
{
}

void pg::Viewport2::mousePressEvent(QMouseEvent* event)
{
	dragPos = event->pos();
	if (event->buttons() & Qt::MidButton)
	{
		dragging = isDraggable;
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
							   (int64_t)((dragPos.x() - event->pos().x()) * dragFacX));
		}
		if (isDraggableY) // Dragging on Y enabled
		{
			rangeY = translate(rangeY, maxRangeY,
							   (int64_t)((dragPos.y() - event->pos().y()) * dragFacY));
		}

		if (isDraggable)
		{
			dragPos = event->pos();
			event->accept();
			repaint();
			Q_EMIT rangeXChanged(rangeX);
			Q_EMIT rangeYChanged(rangeY);
		}
	}
}
void pg::Viewport2::mouseReleaseEvent(QMouseEvent* event)
{
	dragging = false;
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
			int64_t axialX = rasterToAxialX(event->x());
			double fac = std::pow(zoomFacX, steps);
			if (length(rangeX) * fac > width())
			{
				rangeX = scale(rangeX, fac, axialX);
				dragFacX = length(rangeX) / (double) width();
			}
		}
		if (zoomFacY != 1.0)
		{
			int64_t axialY = rasterToAxialY(event->y());
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
		repaint();
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
	int64_t resultXLen = length(rangeX) * event->size().width()
			/ event->oldSize().width();
	int64_t resultYLen = length(rangeX) * event->size().height()
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

void pg::Viewport2::setRangeX(Interval<int64_t> range)
{
	rangeX = range;
	repaint();
}
void pg::Viewport2::setRangeY(Interval<int64_t> range)
{
	rangeY = range;
	repaint();
}