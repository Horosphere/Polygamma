#include "python.hpp"

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "Kernel.hpp"
#include "../singular/BufferSingular.hpp"

namespace pg
{
namespace wrap
{


class Buffer: public pg::Buffer,
	public boost::python::wrapper<pg::Buffer>
{
public:
	Type getType()
	{
		return this->get_override("getType")();
	}
};

}
} // namespace pg::wrap

BOOST_PYTHON_MODULE(pg)
{
	using namespace boost::python;
	def("about", pg::about);

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
	class_<pg::BufferSingular, bases<pg::Buffer>, boost::noncopyable>(
	  "BufferSingular", no_init)
	  .def_readonly("nAudioChannels", &pg::BufferSingular::nAudioChannels);


	class_<pg::Kernel, boost::noncopyable>("Kernel", no_init)
	.def_readonly("buffers", &pg::Kernel::getBuffers);
}

void pg::initPython()
{
	PyImport_AppendInittab("pg", PyInit_pg);
	Py_Initialize();

	using namespace boost::python;
	object dictMain = extract<dict>(import("__main__").attr("__dict__"));
	exec("import pg", dictMain);
}
