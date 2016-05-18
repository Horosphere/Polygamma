#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <QString>

#include "../panels/panelbase.hpp"

namespace pg
{

class Editor : public PanelBase
{
	Q_OBJECT
public:
	explicit Editor(QWidget *parent = 0);
	virtual ~Editor();

	virtual bool saveAs(QString* const error) = 0;

Q_SIGNALS:

public Q_SLOTS:
};

}

#endif // EDITOR_HPP
