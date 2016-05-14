#include "viewport2.hpp"

#include <cmath>

#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QDebug>

pg::Viewport2::Viewport2(QWidget *parent) : QWidget(parent),
	rangeX(0.0, 1.0), rangeY(0.0, 1.0),
	dragFacX(0.0), dragFacY(0.0),
	zoomFacX(1.0), zoomFacY(1.0),
	isDraggable(false), isZoomable(false),

	minSpanX(0.5), minSpanY(0.5),
	maxRangeX(0.0,1.0), maxRangeY(0.0,1.0),
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
		if (dragFacX != 0.0) // Dragging on X enabled
		{
			rangeX += (dragPos.x() - event->pos().x()) * dragFacX;
			if (rangeX.begin < maxRangeX.begin)
				rangeX += maxRangeX.begin - rangeX.begin;
			else if (rangeX.end > maxRangeX.end)
				rangeX += maxRangeX.end - rangeX.end;
		}
		if (dragFacY != 0.0) // Dragging on Y enabled
		{
			rangeY += (dragPos.y() - event->pos().y()) * dragFacY;
			if (rangeY.begin < maxRangeY.begin)
				rangeY += maxRangeY.begin - rangeY.begin;
			else if (rangeY.end > maxRangeY.end)
				rangeY += maxRangeY.end - rangeY.end;
		}

		if (isDraggable)
		{
			dragPos = event->pos();
			event->accept();
			repaint();
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
			double axialX = rasterToAxialX(event->x());
			double fac = std::pow(zoomFacX, steps);
			if (length(rangeX) * fac < minSpanX)
			{
				rangeX.begin = axialX - minSpanX * 0.5;
				rangeX.end = axialX - minSpanX * 0.5;
			}
			else rangeX = scale(rangeX, fac, axialX);
			dragFacX = length(rangeX) / width();
		}
		if (zoomFacY != 1.0)
		{
			double axialY = rasterToAxialY(event->y());
			double fac = std::pow(zoomFacY, steps);
			if (length(rangeX) * fac < minSpanX)
			{
				rangeY.begin = axialY - minSpanY * 0.5;
				rangeY.end = axialY - minSpanY * 0.5;
			}
			else
				rangeY = scale(rangeY, fac, axialY);
			dragFacY = length(rangeY) / height();
		}
	}
	else
	{
		if (zoomFacX != 1.0) // Zoom enabled
		{
			double fac = std::pow(zoomFacX, steps);
			rangeX = scale(rangeX, maxRangeX, fac, rasterToAxialX(event->x()));
			dragFacX = length(rangeX) / width();
		}
		if (zoomFacY != 1.0)
		{
			double fac = std::pow(zoomFacY, steps);
			rangeY = scale(rangeY, maxRangeY, fac, rasterToAxialY(event->y()));
			dragFacY = length(rangeY) / height();
		}
	}
	if (isZoomable)
	{
		event->accept();
		repaint();
	}
}
