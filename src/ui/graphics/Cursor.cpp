#include "Cursor.hpp"

#include <QPainter>

namespace pg
{

Cursor::Cursor(QWidget* parent): QWidget(parent)
{
}

void Cursor::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::red);
}

} // namespace pg
