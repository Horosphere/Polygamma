#ifndef _POLYGAMMA_UI_GRAPHICS_TICKEDAXIS_HPP__
#define _POLYGAMMA_UI_GRAPHICS_TICKEDAXIS_HPP__

#include <QWidget>

namespace pg
{

class TickedAxis final: public QWidget
{
	Q_OBJECT
public:
	explicit TickedAxis(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
};

} // namespace pg

#endif // !_POLYGAMMA_UI_GRAPHICS_TICKEDAXIS_HPP__

