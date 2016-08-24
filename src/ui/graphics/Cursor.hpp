#ifndef _POLYGAMMA_UI_GRAPHICS_CURSOR_HPP__
#define _POLYGAMMA_UI_GRAPHICS_CURSOR_HPP__

#include <QWidget>

namespace pg
{

class Cursor final: public QWidget
{
	Q_OBJECT
public:
	explicit Cursor(QWidget* parent = 0);

protected:
	virtual void paintEvent(QPaintEvent*) override;
};

} // namespace pg

#endif // !_POLYGAMMA_UI_GRAPHICS_CURSOR_HPP__
