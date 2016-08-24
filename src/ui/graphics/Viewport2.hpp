#ifndef _POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__
#define _POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__

#include <cassert>

#include <QPen>
#include <QRubberBand>
#include <QWidget>

#include "../util/Axis.hpp"
#include "../../math/Interval.hpp"

namespace pg
{

/**
 * @brief The Viewport2 class is the base class for editors. It allows
 * navigation by MMB + drag (pan) and scroll (zoom). Viewport2 stores the
 * "view" in two \ref Interval<axial_coord> objects which represent the
 * "view coordinate"
 */
class Viewport2: public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QColor colourBG READ getColourBG WRITE setColourBG)
	Q_PROPERTY(QPen penGrid READ getPenGrid WRITE setPenGrid)
	Q_PROPERTY(QPen penGridMinor READ getPenGridMinor WRITE setPenGridMinor)
public:
	typedef long axial_coord;

	explicit Viewport2(QWidget* parent = 0);

	QColor getColourBG() const noexcept
	{
		return colourBG;
	}
	void setColourBG(QColor c) noexcept
	{
		colourBG = c;
	}
	QPen getPenGrid() const noexcept
	{
		return penGrid;
	}
	void setPenGrid(QPen p) noexcept
	{
		penGrid = p;
	}
	QPen getPenGridMinor() const noexcept
	{
		return penGridMinor;
	}
	void setPenGridMinor(QPen p) noexcept
	{
		penGridMinor = p;
	}

	void setDragging(bool dragX, bool dragY) noexcept;

	/**
	 * @brief setZoomFac Sets the zooming coefficients for this viewport. If
	 * one factor is not 1, then the viewport can be zoomed by scrolling.
	 * @param x The coefficient in the x direction. Must be >= 1.
	 * @param y The coefficient in the y direction. Must be >= 1.
	 */
	void setZoomFac(double x, double y) noexcept;
	void setMaximumRange(axial_coord x0, axial_coord x1,
	                     axial_coord y0, axial_coord y1) noexcept;
	/**
	 * @brief Set the Viewport to the maximum range.
	 */
	void maximise() noexcept;

	/**
	 * The user can draw a selection rectangle by holding LMB. The signals
	 * (true, false): selectionX(Interval<axial_coord>)
	 * (false, true): selectionY(Interval<axial_coord>)
	 * (true, true): selectionXY(Interval<axial_coord>, Interval<axial_coord>)
	 * will be emitted when the selection is made.
	 * @brief Enables the user to draw a selection box on the two dimensions.
	 */
	void setSelecting(bool, bool) noexcept;

	void setAxisX(Axis*) noexcept;
	void setAxisY(Axis*) noexcept;

protected:

	// The following methods convert raster coordinates to axial coordinates
	axial_coord rasterToAxialX(int) const noexcept;
	int axialToRasterX(axial_coord) const noexcept;
	axial_coord rasterToAxialY(int) const noexcept;
	int axialToRasterY(axial_coord) const noexcept;

	// Events (Inherited from QWidget)
	virtual void   mousePressEvent(QMouseEvent*) override;
	virtual void    mouseMoveEvent(QMouseEvent*) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;
	virtual void        wheelEvent(QWheelEvent*) override;
	virtual void       resizeEvent(QResizeEvent*) override;
	virtual void        paintEvent(QPaintEvent*) override;

	Interval<axial_coord> rangeX, rangeY;

Q_SIGNALS:
	// Connect these signals to setRangeX and setRangeY to automatically repaint.
	void rangeXChanged(Interval<axial_coord>);
	void rangeYChanged(Interval<axial_coord>);
	void selectionX(Interval<axial_coord>);
	void selectionY(Interval<axial_coord>);
	void selectionXY(Interval<axial_coord>, Interval<axial_coord>);


public Q_SLOTS:
	// The following slots call repaint()
	void setRangeX(Interval<axial_coord>);
	void setRangeY(Interval<axial_coord>);
	/**
	 * @brief Automatically acquire the x and y intervals from the axes. The axes
	 *  must be instances of AxisInterval for this to work
	 */
	void updateFromAxes();

private:
	QColor colourBG;
	QPen penGrid, penGridMinor;
	// The dragging factors here are calculated based on the ratio of axial
	// width to raster width to increase the precision.
	double dragFacX, dragFacY;
	double zoomFacX, zoomFacY;
	bool isDraggable;
	bool isDraggableX, isDraggableY;
	bool isZoomable;
	bool isSelectibleX, isSelectibleY;
	Axis* axisX;
	Axis* axisY;
	Interval<axial_coord> maxRangeX, maxRangeY;

	// Dynamic variables
	QPoint dragPos;
	QPoint selectTopLeft;
	bool dragging;
	QRubberBand* rubberBand;
};


// Implementations

inline void Viewport2::setDragging(bool dragX, bool dragY) noexcept
{
	isDraggable = dragX || dragY;
	isDraggableX = dragX;
	isDraggableY = dragY;
	dragFacX = length(rangeX) / (double)width();
	dragFacY = length(rangeY) / (double)height();
}
inline void Viewport2::setZoomFac(double x, double y) noexcept
{
	assert(x >= 1.0 && y >= 1.0);
	zoomFacX = x;
	zoomFacY = y;
	isZoomable = zoomFacX != 1.0 || zoomFacY != 1.0;
}
inline void Viewport2::setMaximumRange(axial_coord x0, axial_coord x1,
                                       axial_coord y0, axial_coord y1) noexcept
{
	maxRangeX = Interval<axial_coord>(x0, x1);
	maxRangeY = Interval<axial_coord>(y0, y1);
}
inline void Viewport2::setSelecting(bool x, bool y) noexcept
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
inline void Viewport2::setAxisX(Axis* a) noexcept
{
	axisX = a;
}
inline void Viewport2::setAxisY(Axis* a) noexcept
{
	axisY = a;
}
inline void Viewport2::maximise() noexcept
{
	rangeX = maxRangeX;
	rangeY = maxRangeY;
}

inline Viewport2::axial_coord Viewport2::rasterToAxialX(int v) const noexcept
{
	return v * length(rangeX) / width() + rangeX.begin;
}
inline int Viewport2::axialToRasterX(axial_coord v) const noexcept
{
	return (int)((v - rangeX.begin) * width() / length(rangeX));
}
inline Viewport2::axial_coord Viewport2::rasterToAxialY(int v) const noexcept
{
	return v * length(rangeY) / width() + rangeY.begin;
}
inline int Viewport2::axialToRasterY(axial_coord v) const noexcept
{
	return (int)((v - rangeY.begin) * height() / length(rangeY));
}

inline void Viewport2::setRangeX(Interval<axial_coord> range)
{
	rangeX = range;
	repaint();
}
inline void Viewport2::setRangeY(Interval<axial_coord> range)
{
	rangeY = range;
	repaint();
}

} // namespace pg

#endif // !_POLYGAMMA_UI_GRAPHICS_VIEWPORT2_HPP__

