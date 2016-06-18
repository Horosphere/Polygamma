#include "Kernel.hpp"

#include <thread>
#include <iostream>

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
	struct RedirectorOut
	{
		boost::signals2::signal<void (std::string)>* signal;
		bool newLine;

		void write(std::string const& str)
		{
			if (newLine)
				signal->operator()("[Out] " + str);
			else
				signal->operator()(str);

			newLine = str == "\n";
		}
	} redirectorOut;
	struct RedirectorErr
	{
		boost::signals2::signal<void (std::string)>* signal;
		bool newLine;

		void write(std::string const& str)
		{
			if (newLine)
				signal->operator()("[Err] " + str);
			else
				signal->operator()(str);

			newLine = str == "\n";
		}
	} redirectorErr;
	redirectorOut.signal = redirectorErr.signal = &signalLog;
	redirectorOut.newLine = redirectorErr.newLine = true;	
	dictMain["RedirectorOut"] = boost::python::class_<RedirectorOut>("RedirectorOut",
	                         boost::python::init<>())
	                         .def("write", &RedirectorOut::write);
	dictMain["RedirectorErr"] = boost::python::class_<RedirectorErr>("RedirectorErr",
	                         boost::python::init<>())
	                         .def("write", &RedirectorErr::write);
	boost::python::import("sys").attr("stdout") = redirectorOut;
	boost::python::import("sys").attr("stderr") = redirectorErr;
	

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
			signalLog("\n>> " + command.str);
			
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
