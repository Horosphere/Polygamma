#ifndef _POLYGAMMA_UI_UTIL_ACTIONS_HPP__
#define _POLYGAMMA_UI_UTIL_ACTIONS_HPP__

#include <QAction>

namespace pg
{

/**
 * Emits execute(QString const&) when triggered
 */
class ScriptAction: public QAction
{
	Q_OBJECT
public:
	explicit ScriptAction(QString const& script, QObject* parent = 0);
	explicit ScriptAction(QString const& script, QString const& text,
	                      QObject* parent = 0);
	explicit ScriptAction(QString const& script, QIcon const&,
	                      QString const& text, QObject* parent = 0);

	QString script;

Q_SIGNALS:
	void execute(QString const&);
};

}

#endif // !_POLYGAMMA_UI_UTIL_ACTIONS_HPP__
