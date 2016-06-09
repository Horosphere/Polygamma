#ifndef _POLYGAMMA_UI_EDITORS_EDITOR_HPP__
#define _POLYGAMMA_UI_EDITORS_EDITOR_HPP__

#include <QString>

#include "../panels/PanelBase.hpp"

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

} // namespace pg

#endif // !_POLYGAMMA_UI_EDITORS_EDITOR_HPP__

