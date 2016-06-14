#include "Kernel.hpp"

#include <thread>
#include <iostream>

pg::Kernel::Kernel()
{
}
pg::Kernel::~Kernel()
{
}

void pg::Kernel::start()
{ 
	running = true;
	while (running)
	{
		Command command;
		while (commandQueue.pop(command))
		{
			// Emits the command back to the log listerners for testing purposes
			signalLog(command.str);
			std::cout << command.str << std::endl;
		}
		std::this_thread::yield(); // Avoids busy waiting
	}
	std::cout << "Kernel terminated\n";
}
