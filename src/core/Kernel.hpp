#ifndef _POLYGAMMA_CORE_KERNEL_HPP__
#define _POLYGAMMA_CORE_KERNEL_HPP__

#include <boost/optional.hpp>
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

	struct ScriptOutput
	{
		enum Stream
		{
			StdOut,
			StdErr
		};
		Stream stream;
		std::string string;
	};
	struct SpecialOutput
	{
		enum Type
		{
			Completion, // The previous script execution has finished
			// TODO: Implement config changing in python.
			ConfigUpdate, // Configuration has changed
			BufferNew, // A new buffer has been created
			BufferErase, // A buffer has been deleted
			BufferUpdate // A buffer has been updated
		};
		SpecialOutput() noexcept {}
		SpecialOutput(Type t) noexcept: type(t) {}
		Type type;

		union
		{
			struct
			{
				/*
				 * Valid for: BufferNew, BufferErase
				 */
				Buffer* buffer;
				/*
				 * Valid for: BufferUpdate
				 */
				Buffer::Update update;
			};
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
	 * @warning Do not call this function again until an Output object with type
	 *  Complete is pushed into the output queue.
	 * @brief Execute the script.
	 */
	void execute(Script const&);

	bool popScriptOutput(ScriptOutput* const) noexcept;
	bool popSpecialOutput(SpecialOutput* const) noexcept;
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
	 * Exposed to Python
	 */
	void createSingular(ChannelLayout channelLayout,
	                    std::size_t sampleRate,
	                    std::string duration) throw(PythonException);
	/**
	 * Exposed to Python
	 * @brief "Import" as opposed to "Open" a file. It does not work on internal
	 *  Polygamma project files.
	 */
	void fromFileImport(std::string fileName) throw(PythonException);


private:
	// Not public since they are not thread safe
	/**
	 * Does nothing upon an existing buffer or nullptr is passed. Triggers a
	 * signalBuffer signal.
	 * @brief Add a buffer to the buffers.
	 */
	void pushBuffer(Buffer*) noexcept;
	void streamOut(ScriptOutput::Stream, std::string) noexcept;


	Configuration* config;

	boost::optional<Script> script;
	template <typename Element>
	using Queue = boost::lockfree::spsc_queue<Element, boost::lockfree::capacity<EVENTLOOP_SIZE>>;
	Queue<ScriptOutput> queueOutScript;
	Queue<SpecialOutput> queueOutSpecial;


	std::atomic_bool running;

	// Buffers
	// TODO: Consider if it is necessary to convert this to a set in the future.
	std::vector<Buffer*> buffers;
	boost::python::dict dictMain;
};

// Implementations

inline void
Kernel::halt()
{
	running = false;
}


inline void Kernel::execute(Script const& s)
{
	/*
	 * execute shall not be called when there is a script that has not finished
	 * executing
	 */
	assert(!script);
	script = s;
}
inline bool Kernel::popScriptOutput(ScriptOutput* so) noexcept
{
	return queueOutScript.pop(*so);
}
inline bool Kernel::popSpecialOutput(SpecialOutput* so) noexcept
{
	return queueOutSpecial.pop(*so);
}

inline std::vector<Buffer*>
Kernel::getBuffers() noexcept
{
	return buffers;
}
inline std::size_t
Kernel::bufferIndex(Buffer const* buffer) const noexcept
{
	std::size_t i = 0;
	for (i = 0; i < buffers.size(); ++i)
		if (buffers[i] == buffer) return i;
	return i;
}

// Kernel private
inline void
Kernel::streamOut(ScriptOutput::Stream stream, std::string string) noexcept
{
	queueOutScript.push(ScriptOutput{ScriptOutput::StdOut, string});

}

} // namespace pg
#endif // !_POLYGAMMA_CORE_KERNEL_HPP__
