#ifndef VIEWPORT2_HPP
#define VIEWPORT2_HPP

#include <cassert>

#include <QWidget>

#include "../../math/interval.hpp"

namespace pg
{

/**
 * @brief The Viewport2 class is the base class for editors in Polygamma. It
 * allows navigation by MMB + drag (pan) and scroll (zoom). Viewport2 stores the
 * "view" in two \ref pg::Interval<double> objects which represent the
 * "view coordinate". The burden of converting the view coordinate to raster
 * coordinate lies on the subclasses.
 */
class Viewport2 : public QWidget
{
	Q_OBJECT
public:

	explicit Viewport2(QWidget *parent = 0);
	virtual ~Viewport2();

	void setDragging(bool b)
	{
		isDraggable = b;
		dragFacX = length(rangeX) / width();
		dragFacY = length(rangeY) / height();
	}

	/**
	 * @brief setZoomFac Sets the zooming coefficients for this viewport. If
	 * one factor is not 1, then the viewport can be zoomed by scrolling.
	 * @param x The coefficient in the x direction. Must be >= 1.
	 * @param y The coefficient in the y direction. Must be >= 1.
	 */
	void setZoomFac(double x, double y)
	{
		assert(x >= 1.0 && y >= 1.0);
		zoomFacX = x; zoomFacY = y;
		isZoomable = zoomFacX != 1.0 || zoomFacY != 1.0;
	}
	void setMaximumRange(double x0, double x1, double y0, double y1)
	{
		maxRangeX = Interval<double>(x0, x1);
		maxRangeY = Interval<double>(y0, y1);
	}

protected:

	// The following methods convert raster coordinates to axial coordinates
	double rasterToAxialX(double) const noexcept;
	double axialToRasterX(double) const noexcept;
	double rasterToAxialY(double) const noexcept;
	double axialToRasterY(double) const noexcept;

	// Events (Inherited from QWidget)
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);

	Interval<double> rangeX, rangeY;

Q_SIGNALS:

public Q_SLOTS:

private:

	// The dragging factors here are calculated based on the ratio of axial
	// width to raster width to increase the precision.
	double dragFacX;
	double dragFacY;
	double zoomFacX;
	double zoomFacY;
	bool isDraggable;
	bool isZoomable;

	double minSpanX, minSpanY;
	Interval<double> maxRangeX, maxRangeY;

	// Dynamic variables
	QPoint dragPos;
	bool dragging;
};

}

// Implementations

inline double pg::Viewport2::rasterToAxialX(double v) const noexcept
{
	return v / width() * (rangeX.end - rangeX.begin) + rangeX.begin;
}
inline double pg::Viewport2::axialToRasterX(double v) const noexcept
{
	return (v - rangeX.begin) / (rangeX.end - rangeX.begin) * width();
}
inline double pg::Viewport2::rasterToAxialY(double v) const noexcept
{
	return v / height() * (rangeY.end - rangeY.begin) + rangeY.begin;
}
inline double pg::Viewport2::axialToRasterY(double v) const noexcept
{
	return (v - rangeY.begin) / (rangeY.end - rangeY.begin) * height();
}

#endif // VIEWPORT2_HPP
