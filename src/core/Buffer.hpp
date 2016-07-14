#ifndef _POLYGAMMA_CORE_BUFFER_HPP__
#define _POLYGAMMA_CORE_BUFFER_HPP__

#include <cstdint>
#include <chrono>

#include <boost/signals2.hpp>

#include "python.hpp"
#include "../math/Interval.hpp"

namespace pg
{

typedef Interval<std::size_t> IntervalIndex;

/**
 * TODO: Use reference counted buffer
 * This class uses a case pattern. E.g. if getType() == Singular, the instance
 * must be an instance of BufferSingular.
 * All buffers must be registed in the Kernel
 * @brief Each instance of Buffer corresponds to a file or editable content.
 */
class Buffer
{
public:
	Buffer() noexcept;
	virtual ~Buffer();
	enum Type
	{
		Singular = 1 << 0 // singular/BufferSingular.hpp
	};

	/**
	 * Exposed to Python
	 */
	virtual Type getType() const noexcept = 0;
	/**
	 * Exposed to Python
	 * @brief duration() / timeBase() should be the duration of this buffer in
	 *	seconds
	 */
	virtual std::size_t duration() const noexcept = 0;
	/**
	 * Exposed to Python
	 * Examples:
	 * returns 44100 for 44.1kHz sample audio
	 * returns 24 for 24 fps video
	 * @brief Duration that corresponds to one second.
	 */
	virtual std::size_t timeBase() const noexcept = 0;

	virtual bool saveToFile(std::string fileName, std::string* const error) = 0;
	/**
	 * Exposed to Python. Wraps bool saveToFile(std::string, std::string* const)
	 *  and throws IOError upon failure.
	 */
	void saveToFile(std::string fileName) throw(PythonException);
	std::string getTitle() const noexcept;
	bool isDirty() const noexcept; 


	/**
	 * @brief Sends a notification signal and update timeLastChange.
	 */
	void notifyUpdate() noexcept;
	/**
	 * @brief Sends a notification signal and update timeLastChange.
	 */
	void notifyUpdate(IntervalIndex interval) noexcept;
	/**
	 * @brief Sends a notification signal but does not update timeLastChange.
	 *	Triggered when the selection changes;
	 */
	void notifyUIUpdate() noexcept;
	void uiDestroy() noexcept;

	/**
	 * @brief Registers a listener that is notified (operator()(IntervalIndex))
	 * when the Buffer requires a graphics update.
	 */
	template <typename Listener> void
	registerUpdateListener(Listener listener) const noexcept;
	/**
	 * @brief Registers a listener that is notified (operator()()) when
	 *  the Buffer requires a graphics update.
	 */
	template <typename Listener> void
	registerUIUpdateListener(Listener listener) const noexcept;
	/**
	 * @brief Registers a listener that is notified when the UI corresponding to
	 *  this buffer needs to be closed.
	 */
	template <typename Listener> void
	registerUIDestroyListener(Listener listener) const noexcept;

protected:
	std::string title;
	/**
	 * Used by Buffers that store references to other buffers
	 */
	void referenceIncrease() noexcept;
	void referenceDecrease() noexcept;

private:
	boost::signals2::signal<void (IntervalIndex)> signalUpdate;
	boost::signals2::signal<void ()> signalUIUpdate;
	boost::signals2::signal<void ()> signalUIDestroy;

	std::chrono::time_point<std::chrono::steady_clock> timeLastChange;
	std::chrono::time_point<std::chrono::steady_clock> timeLastSave;
	std::size_t nReferences; // Used by the Kernel
	friend class Kernel;
};

} // namespace pg

// Implementations

inline pg::Buffer::Buffer() noexcept:
	timeLastChange(std::chrono::steady_clock::now()),
	timeLastSave(std::chrono::steady_clock::now()),
	nReferences(0)
{
}
inline std::string pg::Buffer::getTitle() const noexcept
{
	return title;
}
inline bool pg::Buffer::isDirty() const noexcept
{
	return timeLastChange > timeLastSave;
}
inline void pg::Buffer::notifyUpdate() noexcept
{
	timeLastChange = std::chrono::steady_clock::now();
	signalUpdate(IntervalIndex(0, duration()));
}
inline void pg::Buffer::notifyUpdate(IntervalIndex interval) noexcept
{
	timeLastChange = std::chrono::steady_clock::now();
	signalUpdate(interval);
}
inline void pg::Buffer::notifyUIUpdate() noexcept
{
	signalUIUpdate();
}
inline void pg::Buffer::uiDestroy() noexcept
{
	signalUIDestroy();
}
template <typename Listener> inline void
pg::Buffer::registerUpdateListener(Listener listener) const noexcept
{
	const_cast<boost::signals2::signal<void (IntervalIndex)>&>(signalUpdate)
		.connect(listener);
}
template <typename Listener> inline void
pg::Buffer::registerUIUpdateListener(Listener listener) const noexcept
{
	const_cast<boost::signals2::signal<void ()>&>(signalUIUpdate).connect(listener);
}
template <typename Listener> inline void
pg::Buffer::registerUIDestroyListener(Listener listener) const noexcept
{
	const_cast<boost::signals2::signal<void ()>&>(signalUIDestroy).connect(listener);
}

inline void pg::Buffer::referenceIncrease() noexcept
{
	++nReferences;
}
inline void pg::Buffer::referenceDecrease() noexcept
{
	if (nReferences) --nReferences;
}
#endif // !_POLYGAMMA_CORE_BUFFER_HPP__
