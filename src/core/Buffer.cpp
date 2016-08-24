#include "Buffer.hpp"

namespace pg
{

Buffer::~Buffer()
{
}

void Buffer::saveToFile(std::string fileName) throw(PythonException)
{
	std::string error;
	if (!saveToFile(fileName, &error))
	{
		throw PythonException{error, PythonException::IOError};
	}
	dirty = false;
}
void Buffer::exportToFile(std::string fileName) throw(PythonException)
{
	std::string error;
	if (!exportToFile(fileName, &error))
	{
		throw PythonException{error, PythonException::IOError};
	}
	dirty = false;
}

} // namespace pg
