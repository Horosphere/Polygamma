#include <QApplication>
#include <QDebug>

extern "C"
{
#include <libavformat/avformat.h>
}
#include "ui/mainwindow.hpp"

/*
 * Main entry for Polygamma.
 * The main method shall parse the commands, and switch
 * to console mode if it detects -c.
 */
int main(int argc, char* argv[])
{
	av_register_all();
	using namespace pg;
	QApplication application(argc, argv);
	MainWindow window;
	window.show();

	return application.exec();
}

