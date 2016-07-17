#include "Buffer.hpp"

pg::Buffer::~Buffer()
{
}

void pg::Buffer::saveToFile(std::string fileName) throw(PythonException)
{
	std::string error;
	if (!saveToFile(fileName, &error))
	{
		throw PythonException{error, PythonException::IOError};
	}
	dirty = false;
}
void pg::Buffer::exportToFile(std::string fileName) throw(PythonException)
{
	std::string error;
	if (!exportToFile(fileName, &error))
	{
		throw PythonException{error, PythonException::IOError};
	}
	dirty = false;
}
