#ifndef _POLYGAMMA_CORE_KERNEL_HPP__
#define _POLYGAMMA_CORE_KERNEL_HPP__

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/signals2.hpp>

#include "Command.hpp"
#include "Configuration.hpp"
#include "polygamma.hpp"
#include "python.hpp"
#include "Buffer.hpp"
#include "../singular/BufferSingular.hpp"

namespace pg
{

constexpr std::size_t KERNEL_EVENTLOOP_SIZE = 16;

/**
 * This class is responsible for computations.
 * It should be ran in a separate thread.
 *
 * You must call Kernel::halt() to stop the event loop.
 */
class Kernel final
{
public:
	Kernel(Configuration*);
	~Kernel();

	/**
	 * Disabled so the Kernel cannot be copied.
	 */
	Kernel(Kernel const&) = delete;
	Kernel& operator=(Kernel const&) = delete;

	/**
	 * @warning Do not let this function execute concurrently in several threads.
	 * @brief Starts the Kernel event loop. This should be ran in a different
	 *	thread.
	 */
	void start();
	/**
	 * This function can be called multiple times.
	 * @brief Sets a flag that halts the Kernel.
	 */
	void halt();

	/**
	 * @brief Register a callback to receive the stdout output of the kernel.
	 * @tparam Listener A function that accepts (std::string).
	 * @param listener Listener::operator()(std::string) will be called
	 *	occasionally.
	 */
	template<typename Listener> void
	registerStdOutListener(Listener const& listener);
	/**
	 * @brief Register a callback to receive the stdout output of the kernel.
	 * @tparam Listener A function that accepts (std::string).
	 * @param listener Listener::operator()(std::string) will be called
	 *	occasionally.
	 */
	template<typename Listener> void
	registerStdErrListener(Listener const& listener);

	/**
	 * @brief Register a callback to receive the newly created Buffers. The
	 *	ownership of the buffer object remains in the Kernel.
	 * @tparam Listener A function that accepts (Buffer*).
	 * @param listener Listener::operator()(Buffer*) will be called
	 *	occasionally.
	 */
	template<typename Listener> void
	registerBufferListener(Listener const& listener);

	/**
	 * This function shall be called from only one thread.
	 * @brief Pushes a command into the command queue.
	 */
	void pushCommand(Command const&);

	/**
	 * Does nothing upon an existing buffer or nullptr is passed. Triggers a
	 * signalBuffer signal.
	 * @brief Add a buffer to the buffers.
	 */
	void pushBuffer(Buffer*);

	/**
	 * Nothing happens if the erased buffer is nullptr. The buffer is deleted
	 * regardless of whether it is in the buffers or not.
	 * @brief Erases a existing buffer in the buffers.
	 */
	void eraseBuffer(Buffer*);
	

	/**
	 * Exposed to Python
	 * @brief Gets a immutable list of buffers.
	 */
	std::vector<Buffer*> getBuffers() noexcept;

	/**
	 * Exposed to Python 
	 * @brief "Import" as opposed to "Open" a file. It does not work on internal
	 *	Polygamma project files.
	 */
	void fromFileImport(std::string fileName) throw(PythonException);

private:
	Configuration* config;
	

	/**
	 * Concurrent event queue
	 */
	boost::lockfree::spsc_queue<Command, boost::lockfree::capacity<KERNEL_EVENTLOOP_SIZE>> commandQueue;
	// Signals
	boost::signals2::signal<void (std::string)> signalOut;
	boost::signals2::signal<void (std::string)> signalErr;
	boost::signals2::signal<void (Buffer*)> signalBuffer;

	std::atomic_bool running;

	// Buffers
	// TODO: Consider if it is necessary to convert this to a set in the future.
	std::vector<Buffer*> buffers;

	// Python
	boost::python::object moduleMain;
	boost::python::dict dictMain;
};

} // namespace pg


// Implementations

inline void
pg::Kernel::halt()
{
	running = false;
}

template<typename Listener> inline void
pg::Kernel::registerStdOutListener(Listener const& listener)
{
	signalOut.connect(listener);
}
template<typename Listener> inline void
pg::Kernel::registerStdErrListener(Listener const& listener)
{
	signalErr.connect(listener);
}
template<typename Listener> inline void
pg::Kernel::registerBufferListener(Listener const& listener)
{
	signalBuffer.connect(listener);
}

inline void
pg::Kernel::pushCommand(Command const& command)
{
	commandQueue.push(command);
}
inline void
pg::Kernel::pushBuffer(Buffer* buffer)
{
	if (buffer && std::find(buffers.begin(), buffers.end(), buffer) == buffers.end())
	{
		buffers.push_back(buffer);
		signalBuffer(buffer);
	}
}
inline void
pg::Kernel::eraseBuffer(Buffer* buffer)
{
	delete buffer;
	buffers.erase(std::remove(buffers.begin(), buffers.end(), buffer), buffers.end());
}

inline std::vector<pg::Buffer*>
pg::Kernel::getBuffers() noexcept
{
	return buffers;
}
#endif // !_POLYGAMMA_CORE_KERNEL_HPP__
