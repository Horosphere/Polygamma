#include "python.hpp"

#include <cassert>

extern "C"
{
#include <libavutil/channel_layout.h>
}
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "Kernel.hpp"
#include "Buffer.hpp"
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
	.def("getType", pure_virtual(&pg::Buffer::getType))
	.def("saveToFile", (void (pg::Buffer::*)(std::string)) &pg::Buffer::saveToFile);
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
	.def("select", (void (pg::BufferSingular::*)(std::size_t, std::size_t))
	     &pg::BufferSingular::select)
	.def("select", (void (pg::BufferSingular::*)(std::size_t, std::size_t, std::size_t))
	     &pg::BufferSingular::select)
	.def("select", (void (pg::BufferSingular::*)(std::size_t, pg::BufferSingular::AudioInterval))
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

	module.attr("CH_LAYOUT_MONO") = AV_CH_LAYOUT_MONO;
	module.attr("CH_LAYOUT_2POINT1") = AV_CH_LAYOUT_2POINT1;
	module.attr("CH_LAYOUT_2_1") = AV_CH_LAYOUT_2_1;
	module.attr("CH_LAYOUT_SURROUND") = AV_CH_LAYOUT_SURROUND;
	module.attr("CH_LAYOUT_3POINT1") = AV_CH_LAYOUT_3POINT1;
	module.attr("CH_LAYOUT_4POINT0") = AV_CH_LAYOUT_4POINT0;
	module.attr("CH_LAYOUT_4POINT1") = AV_CH_LAYOUT_4POINT1;
	module.attr("CH_LAYOUT_2_2") = AV_CH_LAYOUT_2_2;
	module.attr("CH_LAYOUT_QUAD") = AV_CH_LAYOUT_QUAD;
	module.attr("CH_LAYOUT_5POINT0") = AV_CH_LAYOUT_5POINT0;
	module.attr("CH_LAYOUT_5POINT1") = AV_CH_LAYOUT_5POINT1;
	module.attr("CH_LAYOUT_5POINT0_BACK") = AV_CH_LAYOUT_5POINT0_BACK;
	module.attr("CH_LAYOUT_5POINT1_BACK") = AV_CH_LAYOUT_5POINT1_BACK;
	module.attr("CH_LAYOUT_6POINT0") = AV_CH_LAYOUT_6POINT0;
	module.attr("CH_LAYOUT_6POINT0_FRONT") = AV_CH_LAYOUT_6POINT0_FRONT;
	module.attr("CH_LAYOUT_HEXAGONAL") = AV_CH_LAYOUT_HEXAGONAL;
	module.attr("CH_LAYOUT_6POINT1") = AV_CH_LAYOUT_6POINT1;
	module.attr("CH_LAYOUT_6POINT1_BACK") = AV_CH_LAYOUT_6POINT1_BACK;
	module.attr("CH_LAYOUT_6POINT1_FRONT") = AV_CH_LAYOUT_6POINT1_FRONT;
	module.attr("CH_LAYOUT_7POINT0") = AV_CH_LAYOUT_7POINT0;
	module.attr("CH_LAYOUT_7POINT0_FRONT") = AV_CH_LAYOUT_7POINT0_FRONT;
	module.attr("CH_LAYOUT_7POINT1") = AV_CH_LAYOUT_7POINT1;
	module.attr("CH_LAYOUT_7POINT1_WIDE") = AV_CH_LAYOUT_7POINT1_WIDE;
	module.attr("CH_LAYOUT_7POINT1_WIDE_BACK") = AV_CH_LAYOUT_7POINT1_WIDE_BACK;
	module.attr("CH_LAYOUT_OCTAGONAL") = AV_CH_LAYOUT_OCTAGONAL;
	module.attr("CH_LAYOUT_HEXADECAGONAL") = AV_CH_LAYOUT_HEXADECAGONAL;
	module.attr("CH_LAYOUT_STEREO_DOWNMIX") = AV_CH_LAYOUT_STEREO_DOWNMIX;
}
