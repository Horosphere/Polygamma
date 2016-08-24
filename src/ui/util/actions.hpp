#ifndef _POLYGAMMA_UI_UTIL_ACTIONS_HPP__
#define _POLYGAMMA_UI_UTIL_ACTIONS_HPP__

#include <QAction>

namespace pg
{

class ActionFlagged: public QAction
{
	Q_OBJECT
public:
	typedef int flag_t;
	static constexpr flag_t const FLAG_FULL = 0xFFFFFFFF;

	explicit ActionFlagged(QObject* parent = 0);
	explicit ActionFlagged(QString const& text, QObject* parent = 0);
	explicit ActionFlagged(QIcon const& icon, QString const& text,
	                       QObject* parent = 0);

	/**
	 * @brief An integer that is initialised to FLAG_FULL by default.
	 */
	flag_t flags;
};
/**
 * Emits execute(QString const&) when triggered
 */
class ActionScripted: public ActionFlagged
{
	Q_OBJECT
public:

	explicit ActionScripted(QString const& script, QObject* parent = 0);
	explicit ActionScripted(QString const& script, QString const& text,
	                        QObject* parent = 0);
	explicit ActionScripted(QString const& script, QIcon const&,
	                        QString const& text, QObject* parent = 0);

	QString script;
Q_SIGNALS:
	void execute(QString const&);
};


// Implementations

inline ActionFlagged::ActionFlagged(QObject* parent): QAction(parent),
	flags(FLAG_FULL)
{
}
inline ActionFlagged::ActionFlagged(QString const& text, QObject* parent):
	QAction(text, parent), flags(FLAG_FULL)
{
}
inline ActionFlagged::ActionFlagged(QIcon const& icon, QString const& text,
                                    QObject* parent):
	QAction(icon, text, parent), flags(FLAG_FULL)
{
}

} // namespace pg

#endif // !_POLYGAMMA_UI_UTIL_ACTIONS_HPP__
