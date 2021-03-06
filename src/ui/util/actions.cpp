#include "actions.hpp"

#include <QDialog>

namespace pg
{

// ActionScripted
ActionScripted::ActionScripted(QString const& script, QObject* parent):
	ActionFlagged(parent), script(script)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->script);
	});
}
ActionScripted::ActionScripted(QString const& script, QString const& text,
                               QObject* parent):
	ActionFlagged(text, parent), script(script)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->script);
	});
}
ActionScripted::ActionScripted(QString const& script, QIcon const& icon,
                               QString const& text, QObject* parent):
	ActionFlagged(icon, text, parent), script(script)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->script);
	});
}


} // namespace pg
