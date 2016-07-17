#include "Cursor.hpp"

#include <QPainter>

pg::Cursor::Cursor(QWidget* parent): QWidget(parent)
{
}

void pg::Cursor::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::red);
}
