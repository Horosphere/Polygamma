/*
 * main.cpp
 */

#include <iostream>
#include <thread>

extern "C"
{
#include <libavformat/avformat.h>
#include <SDL2/SDL.h>
}
#include <QApplication>
#include <QStandardPaths>

#include "ui/MainWindow.hpp"

/*
 * Main entry for Polygamma.
 * The main method shall parse the commands, and switch
 * to console mode if it detects -c.
 */
int main(int argc, char* argv[])
{
	// Initialise
	av_register_all();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		std::cerr << "[SDL]" << SDL_GetError() << std::endl;
		return -1;
	}
	if (!pg::initPython())
	{
		std::cerr << "Python initialisation failed" << std::endl;
		return -1;
	}


	// Load config file
	pg::Configuration config;
	std::string configFile = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toStdString() + "/.polygamma";
	config.setFileName(configFile);
	config.loadFile();

	// Launch the Kernel
	pg::Kernel kernel(&config);
	std::thread threadKernel(&pg::Kernel::start, &kernel);
	threadKernel.detach();

	QApplication application(argc, argv);
	application.setApplicationName("Polygamma");

	pg::MainWindow window(&kernel, &config);
	window.show();

	int result = application.exec();

	// Clean up
	kernel.halt();

	return result;
}

