// main.cpp

#include <iostream>
#include <thread>
#include <QApplication>

extern "C"
{
#include <libavformat/avformat.h>
}
#include "ui/MainWindow.hpp"

/*
 * Main entry for Polygamma.
 * The main method shall parse the commands, and switch
 * to console mode if it detects -c.
 */
int main(int argc, char* argv[])
{
	av_register_all();
	pg::initPython();

	pg::Configuration config;
	// Launch the Kernel
	pg::Kernel kernel(&config);
	std::thread threadKernel(&pg::Kernel::start, &kernel);
	threadKernel.detach();


	QApplication application(argc, argv);
	pg::MainWindow window(&kernel, &config);
	window.show();

	return application.exec();
}

