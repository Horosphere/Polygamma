#include "python.hpp"

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "Kernel.hpp"
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
	case pg::PythonException::RuntimeError:
		PyErr_SetString(PyExc_RuntimeError, exc.str.c_str());
		break;
	case pg::PythonException::IOError:
		PyErr_SetString(PyExc_IOError, exc.str.c_str());
		break;
	default:
		PyErr_SetString(PyExc_SystemError,
		                ("Unable to translate: " + exc.str).c_str());
	}
}

class Buffer: public pg::Buffer,
	public boost::python::wrapper<pg::Buffer>
{
public:
	Type getType() const noexcept
	{
		return this->get_override("getType")();
	}
};


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


	// Buffers

	enum_<pg::Buffer::Type>("BufferType")
	.value("Singular", pg::Buffer::Singular);
	class_<pg::wrap::Buffer, boost::noncopyable>("Buffer", no_init)
	.def("getType", pure_virtual(&pg::Buffer::getType));
	class_<std::vector<pg::Buffer*>>("stdvector_Buffer")
	                              .def(vector_indexing_suite<std::vector<pg::Buffer*>>());

	// BufferSingular
	class_<pg::BufferSingular::AudioInterval>("AudioInterval", init<std::size_t, std::size_t>())
	.def_readwrite("begin", &pg::BufferSingular::AudioInterval::first)
	.def_readwrite("end", &pg::BufferSingular::AudioInterval::second);
	class_<pg::BufferSingular, bases<pg::Buffer>, boost::noncopyable>(
	  "BufferSingular", no_init)
	.def_readonly("nAudioChannels", &pg::BufferSingular::nAudioChannels)
	.def_readonly("nAudioSamples", &pg::BufferSingular::nAudioSamples)
	.def("select", (void (pg::BufferSingular::*)(std::size_t, std::size_t, std::size_t))
	     &pg::BufferSingular::select)
	.def("select", (void (pg::BufferSingular::*)(std::size_t, pg::BufferSingular::AudioInterval))
	     &pg::BufferSingular::select)
	.def("clearSelect", &pg::BufferSingular::clearSelect)
	.def("getSelection", &pg::BufferSingular::getSelection);


	class_<pg::Kernel, boost::noncopyable>("Kernel", no_init)
	.def_readonly("buffers", &pg::Kernel::getBuffers)
	.def("fromFileImport", &pg::Kernel::fromFileImport);
}

void pg::initPython()
{
	PyImport_AppendInittab("pg", PyInit_pg);
	Py_Initialize();

	using namespace boost::python;
	object dictMain = extract<dict>(import("__main__").attr("__dict__"));
	exec("import pg", dictMain);
}
