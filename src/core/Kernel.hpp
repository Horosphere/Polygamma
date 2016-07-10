#ifndef _POLYGAMMA_CORE_KERNEL_HPP__
#define _POLYGAMMA_CORE_KERNEL_HPP__

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/signals2.hpp>

#include "Buffer.hpp"
#include "Script.hpp"
#include "Configuration.hpp"
#include "polygamma.hpp"
#include "python.hpp"
#include "../singular/BufferSingular.hpp"

namespace pg
{


/**
 * This class is responsible for computations.
 * It should be ran in a separate thread.
 *
 * You must call Kernel::halt() to stop the event loop.
 */
class Kernel final
{
public:
	static constexpr std::size_t EVENTLOOP_SIZE = 16;
	/**
	 * @warning Currently has no usage.
	 * @brief Each instance represents a special operation for the Kernel.
	 */
	struct Special
	{
		enum Type
		{
			Compute
		} type;
		union
		{
			Buffer* buffer; // type = Deletion
		};
	};

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
	 *  thread.
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
	 *  occasionally.
	 */
	template<typename Listener> void
	registerStdOutListener(Listener const& listener) noexcept;
	/**
	 * @brief Register a callback to receive the stdout output of the kernel.
	 * @tparam Listener A function that accepts (std::string).
	 * @param listener Listener::operator()(std::string) will be called
	 *  occasionally.
	 */
	template<typename Listener> void
	registerStdErrListener(Listener const& listener) noexcept;

	/**
	 * @brief Register a callback to receive the newly created Buffers. The
	 *  ownership of the buffer object remains in the Kernel.
	 * @tparam Listener A function that accepts (Buffer*).
	 * @param listener Listener::operator()(Buffer*) will be called
	 *  occasionally.
	 */
	template<typename Listener> void
	registerBufferListener(Listener const& listener) noexcept;

	/**
	 * This function shall be called from only one thread.
	 * @brief Pushes a script into the script queue.
	 */
	void pushScript(Script const&);
	/**
	 * This function shall be called from only one thread.
	 * @brief Pushes a Special into the Special queue.
	 */
	void pushSpecial(Special const&);

	/**
	 * Exposed to Python
	 * @brief Erases a existing buffer in the buffers.
	 */
	void eraseBuffer(std::size_t index) throw(PythonException);
	/**
	 * Exposed to Python 
	 * @brief Gets a immutable list of buffers.
	 */
	std::vector<Buffer*> getBuffers() noexcept;

	/**
	 * @brief Determine the index of the buffer. Returns the size of the buffer
	 *  if the buffer does not exist in the buffers.
	 */
	std::size_t bufferIndex(Buffer const*) const noexcept;


	/**
	 * Exposed to Script
	 * @brief "Import" as opposed to "Open" a file. It does not work on internal
	 *  Polygamma project files.
	 */
	void fromFileImport(std::string fileName) throw(PythonException);

	void createSingular(ChannelLayout channelLayout,
	                    std::size_t sampleRate,
	                    std::string duration) throw(PythonException);

private:
	// Not public since they are not thread safe
	/**
	 * Does nothing upon an existing buffer or nullptr is passed. Triggers a
	 * signalBuffer signal.
	 * @brief Add a buffer to the buffers.
	 */
	void pushBuffer(Buffer*);


	Configuration* config;


	/**
	 * Concurrent event queue
	 */
	boost::lockfree::spsc_queue<Script, boost::lockfree::capacity<EVENTLOOP_SIZE>> scriptQueue;
	boost::lockfree::spsc_queue<Special, boost::lockfree::capacity<EVENTLOOP_SIZE>> specialQueue;
	// Signals
	boost::signals2::signal<void (std::string)> signalOut;
	boost::signals2::signal<void (std::string)> signalErr;
	boost::signals2::signal<void (Buffer*)> signalBuffer;

	std::atomic_bool running;

	// Buffers
	// TODO: Consider if it is necessary to convert this to a set in the future.
	std::vector<Buffer*> buffers;

	// Script
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
pg::Kernel::registerStdOutListener(Listener const& listener) noexcept
{
	signalOut.connect(listener);
}
template<typename Listener> inline void
pg::Kernel::registerStdErrListener(Listener const& listener) noexcept
{
	signalErr.connect(listener);
}
template<typename Listener> inline void
pg::Kernel::registerBufferListener(Listener const& listener) noexcept
{
	signalBuffer.connect(listener);
}

inline void
pg::Kernel::pushScript(Script const& command)
{
	scriptQueue.push(command);
}
inline void
pg::Kernel::pushSpecial(Special const& command)
{
	specialQueue.push(command);
}

inline std::vector<pg::Buffer*>
pg::Kernel::getBuffers() noexcept
{
	return buffers;
}
inline std::size_t
pg::Kernel::bufferIndex(Buffer const* buffer) const noexcept
{
	std::size_t i = 0;
	for (i = 0; i < buffers.size(); ++i)
		if (buffers[i] == buffer) return i;
	return i;
}

inline void
pg::Kernel::pushBuffer(Buffer* buffer)
{
	if (buffer && std::find(buffers.begin(), buffers.end(), buffer)
			== buffers.end())
	{
		buffers.push_back(buffer);
		signalBuffer(buffer);
	}
}
#endif // !_POLYGAMMA_CORE_KERNEL_HPP__
