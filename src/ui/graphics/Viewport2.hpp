#ifndef _POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__
#define _POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__

#include <cassert>

#include <QWidget>

#include "../../math/Interval.hpp"

namespace pg
{

/**
 * @brief The Viewport2 class is the base class for editors. It allows
 * navigation by MMB + drag (pan) and scroll (zoom). Viewport2 stores the
 * "view" in two \ref pg::Interval<int64_t> objects which represent the
 * "view coordinate"
 */
class Viewport2: public QWidget
{
	Q_OBJECT
public:

	explicit Viewport2(QWidget *parent = 0);
	virtual ~Viewport2();

	void setDragging(bool dragX, bool dragY) noexcept
	{
		isDraggable = dragX || dragY;
		isDraggableX = dragX;
		isDraggableY = dragY;
		dragFacX = length(rangeX) / (double)width();
		dragFacY = length(rangeY) / (double)height();
	}

	/**
	 * @brief setZoomFac Sets the zooming coefficients for this viewport. If
	 * one factor is not 1, then the viewport can be zoomed by scrolling.
	 * @param x The coefficient in the x direction. Must be >= 1.
	 * @param y The coefficient in the y direction. Must be >= 1.
	 */
	void setZoomFac(double x, double y) noexcept
	{
		assert(x >= 1.0 && y >= 1.0);
		zoomFacX = x; zoomFacY = y;
		isZoomable = zoomFacX != 1.0 || zoomFacY != 1.0;
	}
	void setMaximumRange(int64_t x0, int64_t x1, int64_t y0, int64_t y1) noexcept
	{
		maxRangeX = Interval<int64_t>(x0, x1);
		maxRangeY = Interval<int64_t>(y0, y1);
	}
	// Does not redraw
	void maximise()
	{
		rangeX = maxRangeX;
		rangeY = maxRangeY;
	}

protected:

	// The following methods convert raster coordinates to axial coordinates
	int64_t rasterToAxialX(int) const noexcept;
	int axialToRasterX(int64_t) const noexcept;
	int64_t rasterToAxialY(int) const noexcept;
	int axialToRasterY(int64_t) const noexcept;

	// Events (Inherited from QWidget)
	virtual void   mousePressEvent( QMouseEvent*) override;
	virtual void    mouseMoveEvent( QMouseEvent*) override;
	virtual void mouseReleaseEvent( QMouseEvent*) override;
	virtual void        wheelEvent( QWheelEvent*) override;
	virtual void       resizeEvent(QResizeEvent*) override;

	Interval<int64_t> rangeX, rangeY;

Q_SIGNALS:
	void rangeXChanged(Interval<int64_t>);
	void rangeYChanged(Interval<int64_t>);

public Q_SLOTS:
	void setRangeX(Interval<int64_t>);
	void setRangeY(Interval<int64_t>);

private:

	// The dragging factors here are calculated based on the ratio of axial
	// width to raster width to increase the precision.
	double dragFacX;
	double dragFacY;
	double zoomFacX;
	double zoomFacY;
	bool isDraggable;
	bool isDraggableX;
	bool isDraggableY;
	bool isZoomable;

	Interval<int64_t> maxRangeX, maxRangeY;

	// Dynamic variables
	QPoint dragPos;
	bool dragging;
};

} // namespace pg


// Implementations

inline int64_t pg::Viewport2::rasterToAxialX(int v) const noexcept
{
	return v * length(rangeX) / width() + rangeX.begin;
}
inline int pg::Viewport2::axialToRasterX(int64_t v) const noexcept
{
	return (int)((v - rangeX.begin) * width() / length(rangeX));
}
inline int64_t pg::Viewport2::rasterToAxialY(int v) const noexcept
{
	return v * length(rangeY) / width() + rangeY.begin;
}
inline int pg::Viewport2::axialToRasterY(int64_t v) const noexcept
{
	return (int)((v - rangeY.begin) * height() / length(rangeY));
}

#endif // !_POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__

