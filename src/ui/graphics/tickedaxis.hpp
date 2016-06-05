#ifndef TICKEDAXIS_HPP
#define TICKEDAXIS_HPP

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

#endif // TICKEDAXIS_HPP
