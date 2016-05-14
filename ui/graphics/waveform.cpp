#include "waveform.hpp"

#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

pg::Waveform::Waveform(QWidget* parent): Viewport2(parent)
{
	setMinimumSize(400,400);
	setDragging(true);
	setZoomFac(1.1, 1.1);
	setMaximumRange(-0.5, 1.5, -0.5, 1.5);
}

void pg::Waveform::paintEvent(QPaintEvent*)
{

	QPainter painter(this);
	QRect bar = rect();
	bar.setLeft(axialToRasterX(0.25));
	bar.setRight(axialToRasterX(0.75));
	bar.setTop(axialToRasterY(0.25));
	bar.setBottom(axialToRasterY(0.75));
	painter.fillRect(bar, Qt::black);
}
