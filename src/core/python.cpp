#include "python.hpp"

#include "Kernel.hpp"

BOOST_PYTHON_MODULE(pg)
{
	using namespace boost::python;
	def("about", pg::about);

	class_<pg::Kernel, boost::noncopyable>("Kernel", no_init)
		.def("nBuffers", &pg::Kernel::nBuffers);
}

void pg::initPython()
{
	PyImport_AppendInittab("pg", PyInit_pg);
	Py_Initialize();
	
	using namespace boost::python;
	object dictMain = extract<dict>(import("__main__").attr("__dict__"));
	exec("import pg", dictMain);
}
