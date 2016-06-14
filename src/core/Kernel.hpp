#ifndef _POLYGAMMA_CORE_KERNEL_HPP__
#define _POLYGAMMA_CORE_KERNEL_HPP__

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include "Command.hpp"

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
	 * @brief Starts the Kernel event loop. This should be ran in a different
	 *	thread.
	 */
	void start();
	/**
	 * @brief Sets a flag that halts the Kernel.
	 */
	void halt();

	/**
	 * @brief Register a callback to receive the console output of the kernel.
	 * @tparam Listener A function that accepts (std::string).
	 * @param listener Listener::operator()(std::string) will be called
	 *	occasionally.
	 */
	template<typename Listener> void
	registerLogListener(Listener const& listener);

	/**
	 * This function shall be called from only one thread.
	 * @brief Pushes a command into the command queue.
	 */
	void pushCommand(Command const&);

private:

	/**
	 * Concurrent event queue
	 */
	boost::lockfree::spsc_queue<Command, boost::lockfree::capacity<KERNEL_EVENTLOOP_SIZE>> commandQueue;
	boost::signals2::signal<void (std::string)> signalLog;

	bool running;
};

} // namespace pg


// Implementations

inline void
pg::Kernel::halt()
{
	running = false;
}

template<typename Listener> inline void
pg::Kernel::registerLogListener(Listener const& func)
{
	signalLog.connect(func);
}

inline void
pg::Kernel::pushCommand(Command const& command)
{
	commandQueue.push(command);
}


#endif // !_POLYGAMMA_CORE_KERNEL_HPP__
