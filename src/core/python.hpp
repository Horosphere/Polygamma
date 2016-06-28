#ifndef _POLYGAMMA_CORE_PYTHON_HPP__
#define _POLYGAMMA_CORE_PYTHON_HPP__

#include <Python.h>
#include <boost/python.hpp>

namespace pg
{


void initPython();

/**
 * @warning This function uses global functions from the python library and is
 *	thus not pure. It does not have any side effects though.
 * @brief Produces a traceback string of the python exception.
 */
std::string pythonTraceBack();

struct PythonException
{
	enum Type
	{
		RuntimeError,
		IOError
	};

	std::string str;
	Type type;
};

/**
 * The command that references the Polygamma Kernel in Python.
 */
constexpr char PYTHON_KERNEL[] = "pg.kernel";

} // namespace pg

// Implementations


inline std::string
pg::pythonTraceBack()
{
	using namespace boost::python;

	PyObject* exType;
	PyObject* value;
	PyObject* traceBack;
	PyErr_Fetch(&exType, &value, &traceBack);
	// This can happen if the user entered print(
	if (exType == nullptr || value == nullptr || traceBack == nullptr)
		return "Unknown Error\n";
	// Needed to prevent exception from being thrown.
	PyErr_NormalizeException(&exType, &value, &traceBack);
	// This is the Python3 way to do it;
	object oExType(handle<>(borrowed(exType)));
	object oValue(handle<>(borrowed(value)));
	object oTraceBack(handle<>(borrowed(traceBack)));
	object lines = import("traceback").attr("format_exception")
	               (oExType, oValue, oTraceBack);
	std::string result;
	for (int i = 0; i < len(lines); ++i)
		result += extract<std::string>(lines[i])();

	PyErr_Restore(exType, value, traceBack);
	return result;
}

#endif // !_POLYGAMMA_CORE_PYTHON_HPP__
