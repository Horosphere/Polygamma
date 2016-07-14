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
	timeLastSave = std::chrono::steady_clock::now();
	dirty = false;
}
