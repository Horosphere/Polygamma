#include "python.hpp"

#include <cassert>

extern "C"
{
#include <libavutil/channel_layout.h>
}
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "Kernel.hpp"
#include "Buffer.hpp"
#include "text.hpp"
#include "../singular/audio.hpp"
#include "../singular/BufferSingular.hpp"

namespace pg
{
namespace wrap
{
// Within this namespace is the only place where function names can begin with
// capital letters.

void exceptionTranslator(pg::PythonException const& exc)
{
	switch (exc.type)
	{
	case pg::PythonException::IndexError:
		PyErr_SetString(PyExc_IndexError, exc.str.c_str());
		break;
	case pg::PythonException::ValueError:
		PyErr_SetString(PyExc_ValueError, exc.str.c_str());
		break;
	case pg::PythonException::RuntimeError:
		PyErr_SetString(PyExc_RuntimeError, exc.str.c_str());
		break;
	case pg::PythonException::IOError:
		PyErr_SetString(PyExc_IOError, exc.str.c_str());
		break;
	default:
		assert(false && "Unable to translate Python exception");
		PyErr_SetString(PyExc_SystemError,
		                ("Unable to translate: " + exc.str).c_str());
	}
}


}
} // namespace pg::wrap

BOOST_PYTHON_MODULE(pg)
{
	// Do not expose any factory method that returns a reference/pointer. Leaks
	// will occur if the user is sloppy.

	using namespace boost::python;
	def("about", pg::about);

	register_exception_translator<pg::PythonException>(pg::wrap::exceptionTranslator);

	// Naming convention for classes: Use underscores for template parameters.
	class_<pg::Vector<pg::real>>("Vector_real")
	                          .def(init<std::size_t>())
	                          .def("__len__", &pg::Vector<pg::real>::getSize)
	                          .def("__getitem__", (pg::real(pg::Vector<pg::real>::*)(std::size_t) const)
	                               &pg::Vector<pg::real>::operator[]);
	class_<std::vector<pg::Vector<pg::real>>>("stdvector_Vector_real")
	.def(vector_indexing_suite<std::vector<pg::Vector<pg::real>>>());

	class_<pg::IntervalIndex>("IntervalIndex", init<std::size_t, std::size_t>())
	.def_readwrite("begin", &pg::IntervalIndex::begin)
	.def_readwrite("end", &pg::IntervalIndex::end);

	// Buffers

	/*
	 * Although pg::Buffer is a pure virtual class, its methods are not wrapped
	 * since the user will not be inherting from it.
	 */
	enum_<pg::Buffer::Type>("BufferType")
	.value("Singular", pg::Buffer::Singular);
	class_<pg::Buffer, boost::noncopyable>("Buffer", no_init)
	.def_readonly("getType", &pg::Buffer::getType)
	.def_readonly("duration", &pg::Buffer::duration)
	.def_readonly("timeBase", &pg::Buffer::timeBase)
	.def("saveToFile", (void (pg::Buffer::*)(std::string))
				&pg::Buffer::saveToFile)
	.def("exportToFile", (void (pg::Buffer::*)(std::string))
				&pg::Buffer::exportToFile)
	.add_property("title", &pg::Buffer::getTitle)
	.add_property("dirty", &pg::Buffer::isDirty)
	.add_property("cursor", &pg::Buffer::getCursor, &pg::Buffer::setCursor);
	class_<std::vector<pg::Buffer*>>("stdvector_Buffer")
	                              .def(vector_indexing_suite<std::vector<pg::Buffer*>>());

	// BufferSingular
	class_<pg::BufferSingular, bases<pg::Buffer>, boost::noncopyable>(
	  "BufferSingular", no_init)
	.def("select", (void (pg::BufferSingular::*)(std::size_t, std::size_t))
	     &pg::BufferSingular::select)
	.def("select", (void (pg::BufferSingular::*)(std::size_t, std::size_t, std::size_t))
	     &pg::BufferSingular::select)
	.def("select", (void (pg::BufferSingular::*)(std::size_t, pg::IntervalIndex))
	     &pg::BufferSingular::select)
	.def("clearSelect", (void (pg::BufferSingular::*)())
			&pg::BufferSingular::clearSelect)
	.def("clearSelect", (void (pg::BufferSingular::*)(std::size_t))
			&pg::BufferSingular::clearSelect)
	.def("getSelection", &pg::BufferSingular::getSelection);

	// BufferSingular associated functions
	def("silence", (void (*)(pg::BufferSingular*)) &pg::silence);

	class_<pg::Kernel, boost::noncopyable>("Kernel", no_init)
	.def_readonly("buffers", &pg::Kernel::getBuffers)
	.def("fromFileImport", &pg::Kernel::fromFileImport)
	.def("eraseBuffer", &pg::Kernel::eraseBuffer)
	.def("createSingular", &pg::Kernel::createSingular);

	// If this line does not show, then the above code has thrown an exception
	std::cout << "Python module initialisation complete" << std::endl;
}

void pg::initPython()
{
	PyImport_AppendInittab("pg", PyInit_pg);
	Py_Initialize();

	using namespace boost::python;
	object module = import("pg");

	for (auto const& channelName: channelNames)
		module.attr(boost::get<2>(channelName).c_str()) = boost::get<0>(channelName);
}
