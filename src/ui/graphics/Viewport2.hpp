#ifndef _POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__
#define _POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__

#include <cassert>

#include <QPen>
#include <QRubberBand>
#include <QWidget>

#include "../../math/Interval.hpp"

namespace pg
{

/**
 * @brief The Viewport2 class is the base class for editors. It allows
 * navigation by MMB + drag (pan) and scroll (zoom). Viewport2 stores the
 * "view" in two \ref pg::Interval<long> objects which represent the
 * "view coordinate"
 */
class Viewport2: public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QPen penGrid READ getPenGrid WRITE setPenGrid)
	Q_PROPERTY(QPen penGridMinor READ getPenGridMinor WRITE setPenGridMinor)
public:
	explicit Viewport2(QWidget* parent = 0);

	QPen getPenGrid() const noexcept { return penGrid; }
	void setPenGrid(QPen p) noexcept { penGrid = p; }
	QPen getPenGridMinor() const noexcept { return penGridMinor; }
	void setPenGridMinor(QPen p) noexcept { penGridMinor = p; }

	void setDragging(bool dragX, bool dragY) noexcept;

	/**
	 * @brief setZoomFac Sets the zooming coefficients for this viewport. If
	 * one factor is not 1, then the viewport can be zoomed by scrolling.
	 * @param x The coefficient in the x direction. Must be >= 1.
	 * @param y The coefficient in the y direction. Must be >= 1.
	 */
	void setZoomFac(double x, double y) noexcept;
	void setMaximumRange(long x0, long x1,
	                     long y0, long y1) noexcept;
	/**
	 * @brief Set the Viewport to the maximum range.
	 */
	void maximise() noexcept;

	/**
	 * The user can draw a selection rectangle by holding LMB. The signals
	 * (true, false): selectionX(Interval<long>)
	 * (false, true): selectionY(Interval<long>)
	 * (true, true): selectionXY(Interval<long>, Interval<long>)
	 * will be emitted when the selection is made.
	 * @brief Enables the user to draw a selection box on the two dimensions.
	 */
	void setSelecting(bool, bool) noexcept;

protected:

	// The following methods convert raster coordinates to axial coordinates
	long rasterToAxialX(int) const noexcept;
	int axialToRasterX(long) const noexcept;
	long rasterToAxialY(int) const noexcept;
	int axialToRasterY(long) const noexcept;

	// Events (Inherited from QWidget)
	virtual void   mousePressEvent(QMouseEvent*) override;
	virtual void    mouseMoveEvent(QMouseEvent*) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;
	virtual void        wheelEvent(QWheelEvent*) override;
	virtual void       resizeEvent(QResizeEvent*) override;

	Interval<long> rangeX, rangeY;

Q_SIGNALS:
	void rangeXChanged(Interval<long>);
	void rangeYChanged(Interval<long>);
	void selectionX(Interval<long>);
	void selectionY(Interval<long>);
	void selectionXY(Interval<long>, Interval<long>);


public Q_SLOTS:
	void setRangeX(Interval<long>);
	void setRangeY(Interval<long>);

private:
	QPen penGrid, penGridMinor;
	// The dragging factors here are calculated based on the ratio of axial
	// width to raster width to increase the precision.
	double dragFacX, dragFacY;
	double zoomFacX, zoomFacY;
	bool isDraggable;
	bool isDraggableX, isDraggableY;
	bool isZoomable;
	bool isSelectibleX, isSelectibleY;

	Interval<long> maxRangeX, maxRangeY;

	// Dynamic variables
	QPoint dragPos;
	QPoint selectTopLeft;
	bool dragging;
	QRubberBand* rubberBand;
};

} // namespace pg


// Implementations

inline void pg::Viewport2::setDragging(bool dragX, bool dragY) noexcept
{
	isDraggable = dragX || dragY;
	isDraggableX = dragX;
	isDraggableY = dragY;
	dragFacX = length(rangeX) / (double)width();
	dragFacY = length(rangeY) / (double)height();
}
inline void pg::Viewport2::setZoomFac(double x, double y) noexcept
{
	assert(x >= 1.0 && y >= 1.0);
	zoomFacX = x;
	zoomFacY = y;
	isZoomable = zoomFacX != 1.0 || zoomFacY != 1.0;
}
inline void pg::Viewport2::setMaximumRange(long x0, long x1,
    long y0, long y1) noexcept
{
	maxRangeX = Interval<long>(x0, x1);
	maxRangeY = Interval<long>(y0, y1);
}
inline void pg::Viewport2::setSelecting(bool x, bool y) noexcept
{
	isSelectibleX = x;
	isSelectibleY = y;
	if (isSelectibleX || isSelectibleY)
		rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	else 
	{
		delete rubberBand;
		rubberBand = nullptr;
	}
}
inline void pg::Viewport2::maximise() noexcept
{
	rangeX = maxRangeX;
	rangeY = maxRangeY;
}

inline long pg::Viewport2::rasterToAxialX(int v) const noexcept
{
	return v * length(rangeX) / width() + rangeX.begin;
}
inline int pg::Viewport2::axialToRasterX(long v) const noexcept
{
	return (int)((v - rangeX.begin) * width() / length(rangeX));
}
inline long pg::Viewport2::rasterToAxialY(int v) const noexcept
{
	return v * length(rangeY) / width() + rangeY.begin;
}
inline int pg::Viewport2::axialToRasterY(long v) const noexcept
{
	return (int)((v - rangeY.begin) * height() / length(rangeY));
}

inline void pg::Viewport2::setRangeX(Interval<long> range)
{
	rangeX = range;
	repaint();
}
inline void pg::Viewport2::setRangeY(Interval<long> range)
{
	rangeY = range;
	repaint();
}
#endif // !_POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__

