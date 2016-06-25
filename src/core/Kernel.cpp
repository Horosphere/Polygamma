#include "Kernel.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

pg::Kernel::Kernel(Configuration* config): config(config)
{

	// Must be placed here instead of the initialiser list to avoid crashing.
	moduleMain = boost::python::import("__main__");
	dictMain = boost::python::extract<boost::python::dict>(
	             moduleMain.attr("__dict__"));

	std::cout << "Kernel starting..." << std::endl;
	// Sets the Kernel variable in the Polygamma module. The Kernel can be
	// accessed in python with pg.kernel.
	boost::python::import("pg").attr("kernel") = boost::ref(*this);

	// Redirects Python stdout and stderr streams to the terminal
	class Redirector
	{
	public:
		Redirector(boost::signals2::signal<void (std::string)>* signal,
		           std::string prepend):
			signal(signal), prepend(prepend), newLine(true)
		{
		}
		void write(std::string const& str)
		{
			if (str == "") return;
			std::string result;
			std::stringstream ss(str);
			std::string item;
			std::getline(ss, item, '\n');
			if (newLine)
				result += prepend;
			result += item;
			while (std::getline(ss, item, '\n'))
				result += '\n' + prepend + item;

			newLine = str.back() == '\n';
			if (newLine) result += '\n';

			signal->operator()(result);
		}
	private:
		boost::signals2::signal<void (std::string)>* signal;
		std::string prepend;
		bool newLine;
	};
	dictMain["Redirector"] = boost::python::class_<Redirector>("Redirector",
	                         boost::python::no_init)
	                         .def("write", &Redirector::write);
	boost::python::import("sys").attr("stdout") =
	  Redirector(&signalOut, "[Out] ");
	boost::python::import("sys").attr("stderr") =
	  Redirector(&signalErr, "[Err] ");


}
pg::Kernel::~Kernel()
{
	// Releases all buffers
	for (auto const& buffer: buffers)
		delete buffer;
}

void pg::Kernel::start()
{
	running = true;
	while (running)
	{
		Command command;
		while (commandQueue.pop(command))
		{
			try
			{
				using namespace boost::python;

				object result = eval(command.str.c_str(), dictMain);
				if (!result.is_none())
					signalOut(extract<std::string>(str(result))());
			}
			catch (boost::python::error_already_set const&)
			{
				signalErr(pythonTraceBack());
				PyErr_Clear();
			}
		}
		std::this_thread::yield(); // Avoids busy waiting
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


void pg::Kernel::fromFileImport(std::string fileName) throw(PythonException)
{
	// TODO: Prevent user from importing the same file multiple times.
	std::string error;
	BufferSingular* buffer = BufferSingular::fromFile(fileName, &error);
	if (buffer) pushBuffer(buffer);
	else
		throw PythonException{error, PythonException::IOError};
}
