#include "Kernel.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "text.hpp"

pg::Kernel::Kernel(Configuration* config): config(config)
{
	// Must be placed here instead of the initialiser list to avoid crashing.
	boost::python::object moduleMain = boost::python::import("__main__");
	dictMain = boost::python::extract<boost::python::dict>(
	             moduleMain.attr("__dict__"));
}
pg::Kernel::~Kernel()
{
	// Releases all buffers
	// TODO: Call each buffer's destruction signal
	for (auto const& buffer: buffers)
		delete buffer;
}

void pg::Kernel::start()
{
	// Sets the Kernel variable in the Polygamma module. The Kernel can be
	// accessed in python with PYTHON_KERNEL
	// std::ref cannot be used here since it doesn't have the interface of
	// boost::ref
	boost::python::import("pg").attr("kernel") = boost::ref(*this);

	// Redirects Script stdout and stderr streams to the terminal
	class Redirector
	{
	public:
		Redirector(std::function<void (std::string)> func,
		           std::string prepend):
			func(func), prepend(prepend), newLine(true)
		{
		}
		void write(std::string const& str)
		{
			if (str == "") return;
			std::string result;
			std::stringstream ss(str);
			std::string item;
			std::getline(ss, item, '\n');
			if (newLine)
				result += prepend;
			result += item;
			while (std::getline(ss, item, '\n'))
				result += '\n' + prepend + item;

			newLine = str.back() == '\n';
			if (newLine) result += '\n';

			func(result);
		}
	private:
		std::function<void (std::string)> func;
		std::string prepend;
		bool newLine;
	};
	dictMain["Redirector"] = boost::python::class_<Redirector>("Redirector",
	                         boost::python::no_init)
	                         .def("write", &Redirector::write);
	boost::python::import("sys").attr("stdout") =
	  Redirector([this](std::string str)
	{
		this->streamOut(ScriptOutput::StdOut, str);
	} , "[Out] ");
	boost::python::import("sys").attr("stderr") =
	  Redirector([this](std::string str)
	{
		this->streamOut(ScriptOutput::StdErr, str);
	} , "[Err] ");
	boost::python::exec("import pg", dictMain);

	std::cout << "[Ker] starting..." << std::endl;

	running = true;
	while (running)
	{
		if (script)
		{
			if (script->level != Script::System)
			{
				std::cout << "[Ker] " << (std::string)(*script) << std::endl;
			}
			try
			{
				using namespace boost::python;

				object result = eval(((std::string)(*script)).c_str(), dictMain);
				if (!result.is_none())
					streamOut(ScriptOutput::StdOut, extract<std::string>(str(result))());
			}
			catch (boost::python::error_already_set&)
			{
				streamOut(ScriptOutput::StdErr, pythonTraceBack());
				PyErr_Clear();
			}
			queueOutSpecial.push(SpecialOutput(SpecialOutput::Completion));
			script = boost::none;
		}
		std::this_thread::yield(); // Avoids busy waiting
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "[Ker] stopping..." << std::endl;
}

void pg::Kernel::eraseBuffer(std::size_t index) throw(PythonException)
{
	if (buffers.size() <= index)
		throw PythonException{"Buffer index out of range", PythonException::ValueError};

	Buffer* const buffer = buffers[index];
	if (buffer->nReferences == 0)
	{
		std::cout << "[Ker] Erase buffer: " << buffer->getTitle() << std::endl;
		buffers.erase(buffers.begin() + index);
		SpecialOutput so(SpecialOutput::BufferErase);
		so.buffer = buffer;
		queueOutSpecial.push(so);

		delete buffer;
	}
}
void pg::Kernel::createSingular(ChannelLayout channelLayout,
                                std::size_t sampleRate,
                                std::string duration) throw(PythonException)
{
	std::string error;

	std::size_t d = stringToTimePoint(duration) * sampleRate;
	BufferSingular* buffer = BufferSingular::create(channelLayout, sampleRate, d,
	                         &error);
	if (buffer) pushBuffer(buffer);
	else throw PythonException{error, PythonException::ValueError};
}
void pg::Kernel::fromFileImport(std::string fileName) throw(PythonException)
{
	// TODO: Prevent user from importing the same file multiple times.
	std::string error;
	BufferSingular* buffer = BufferSingular::fromFile(fileName, &error);
	if (buffer) pushBuffer(buffer);
	else throw PythonException{error, PythonException::IOError};
}

void pg::Kernel::pushBuffer(Buffer* buffer) noexcept
{
	if (buffer && std::find(buffers.begin(), buffers.end(), buffer)
	    == buffers.end())
	{
		std::cout << "[Ker] Create buffer: " << buffer->getTitle() << std::endl;
		buffers.push_back(buffer);
		SpecialOutput so(SpecialOutput::BufferNew);
		so.buffer = buffer;
		queueOutSpecial.push(so);

		buffer->signalUpdate.connect([buffer, this](Buffer::Update update)
		{
			SpecialOutput so(SpecialOutput::BufferUpdate);
			so.buffer = buffer;
			so.update = update;
			queueOutSpecial.push(so);
		});
	}
}
