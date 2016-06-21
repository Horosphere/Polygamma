#ifndef _POLYGAMMA_CORE_KERNEL_HPP__
#define _POLYGAMMA_CORE_KERNEL_HPP__

#include <atomic>

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/signals2.hpp>

#include "Command.hpp"
#include "Configuration.hpp"
#include "polygamma.hpp"
#include "python.hpp"

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
	Kernel();
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
	 * This function shall be called from only one thread.
	 * @brief Pushes a command into the command queue.
	 */
	void pushCommand(Command const&);

	Configuration config;

	std::size_t nBuffers() const;
private:

	/**
	 * Concurrent event queue
	 */
	boost::lockfree::spsc_queue<Command, boost::lockfree::capacity<KERNEL_EVENTLOOP_SIZE>> commandQueue;
	boost::signals2::signal<void (std::string)> signalOut;
	boost::signals2::signal<void (std::string)> signalErr;

	std::atomic_bool running;

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

inline void
pg::Kernel::pushCommand(Command const& command)
{
	commandQueue.push(command);
}

inline std::size_t
pg::Kernel::nBuffers() const
{
	return 0;
}

#endif // !_POLYGAMMA_CORE_KERNEL_HPP__
