#include "Kernel.hpp"

#include <iostream>
#include <thread>
#include <sstream>

#include <Python.h>
#include <boost/python.hpp>

namespace pg
{
/**
 * @warning This function is not pure and uses global functions from the python
 *	library.
 * @brief Produces a traceback string of the python exception.
 */
std::string pythonTraceBack(boost::python::error_already_set const& error);
} // namespace pg

std::string pg::pythonTraceBack(boost::python::error_already_set const& error)
{
	(void) error;

	using namespace boost::python;
	std::string result = "";

	PyObject* exType;
	PyObject* value;
	PyObject* traceBack;
	PyErr_Fetch(&exType, &value, &traceBack);
	object objectExType(handle<>(borrowed(exType)));
	object objectValue(handle<>(borrowed(value)));
	object objectTraceBack(handle<>(borrowed(traceBack)));
	object moduleTraceBack = import("traceback");
	object lines = moduleTraceBack.attr("format_exception")(objectExType, objectValue, objectTraceBack);

	for (int i = 0; i < len(lines); ++i)
		result.append(extract<std::string>(lines[i])());

	return result;
}

pg::Kernel::Kernel(): config()
{
	Py_Initialize();
	moduleMain = boost::python::import("__main__");
	dictMain = moduleMain.attr("__dict__");

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
		Redirector(&signalLog, "[Out] ");
	boost::python::import("sys").attr("stderr") =
		Redirector(&signalLog, "[Err] ");;


}
pg::Kernel::~Kernel()
{
	// Boost.Python does not allow this
	// Py_Finalize();
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
			signalLog(">> " + command.str + "\n");

			try
			{
				boost::python::object ignored =
				    boost::python::exec(command.str.c_str(), dictMain);
			}
			catch (boost::python::error_already_set& error)
			{
				signalLog(pythonTraceBack(error));
			}


		}
		std::this_thread::yield(); // Avoids busy waiting
	}
}
