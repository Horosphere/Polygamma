#ifndef SETTINGSHANDLER_HPP
#define SETTINGSHANDLER_HPP

#include <QObject>

class SettingsHandler : public QObject
{
	Q_OBJECT
public:
	explicit SettingsHandler(QObject *parent = 0);

signals:

public slots:
};

#endif // SETTINGSHANDLER_HPP