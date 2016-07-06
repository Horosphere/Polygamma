#include "actions.hpp"

// ScriptAction
pg::ScriptAction::ScriptAction(QString const& script, QObject* parent):
	QAction(parent), script(script), flags(0xFFFFFFFF)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->script);
	});
}
pg::ScriptAction::ScriptAction(QString const& script, QString const& text,
                               QObject* parent):
	QAction(text, parent), script(script), flags(0xFFFFFFFF)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->script);
	});
}
pg::ScriptAction::ScriptAction(QString const& script, QIcon const& icon,
                               QString const& text, QObject* parent):
	QAction(icon, text, parent), script(script), flags(0xFFFFFFFF)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->script);
	});
}

