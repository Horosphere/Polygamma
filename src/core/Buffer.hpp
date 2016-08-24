#ifndef _POLYGAMMA_CORE_BUFFER_HPP__
#define _POLYGAMMA_CORE_BUFFER_HPP__

#include <cstdint>

#include <boost/signals2.hpp>

#include "python.hpp"
#include "../math/Interval.hpp"

namespace pg
{

typedef Interval<std::size_t> IntervalIndex;
constexpr IntervalIndex const INTERVALINDEX_NULL(std::numeric_limits<std::size_t>::max(), 0);

/**
 * TODO: Use reference counted buffer
 * This class uses a case pattern. E.g. if getType() == Singular, the instance
 * must be an instance of BufferSingular.
 * It is suggested to use dynamic_cast to determine the buffer type instead of
 * getType(). getType() is mainly used in the GUI for disabling certain
 * actions.
 * @brief Each instance of Buffer corresponds to a file or editable content.
 */
class Buffer
{
public:
	struct Update
	{
		enum Level
		{
			Data, // The underlying data has been modified
			Surface // Only the selection/cursor has been modified
		};

		Level level;
		IntervalIndex indices;
	};

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
	 *  seconds
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

	/**
	 * @warning Do not change the dirty flag within this function
	 * @brief Export as a project file
	 */
	virtual bool saveToFile(std::string fileName,
	                        std::string* const error) const noexcept = 0;
	/**
	 * @warning Do not change the dirty flag within this function
	 * @brief Export as a playable multimedia file
	 */
	virtual bool exportToFile(std::string fileName,
	                          std::string* const error) const noexcept = 0;
	/**
	 * Exposed to Python
	 * @brief Plays the buffer. Subclass implementations should call the parent
	 */
	virtual void play() throw(PythonException);
	/**
	 * Exposed to Python
	 * @brief Stops the buffer. Subclass implementations should call the parent
	 */
	virtual void stop() throw(PythonException);
	/**
	 * Exposed to Python
	 */
	virtual bool playing() const noexcept = 0;
	/**
	 * Exposed to Python. Wraps bool saveToFile(std::string, std::string* const)
	 *  and throws IOError upon failure.
	 */
	void saveToFile(std::string fileName) throw(PythonException);
	void exportToFile(std::string fileName) throw(PythonException);
	std::string getTitle() const noexcept;
	bool isDirty() const noexcept;

	std::size_t getCursor() const noexcept;
	/**
	 * The argument must be less than duration().
	 */
	void setCursor(std::size_t) throw(PythonException);
	/**
	 * @brief Sends a notification signal and update timeLastChange.
	 */
	void notifyUpdate(Update::Level) noexcept;
	/**
	 * @brief Sends a notification signal and update timeLastChange.
	 */
	void notifyUpdate(Update::Level, IntervalIndex interval) noexcept;


protected:
	std::string title;
	/**
	 * Used by Buffers that store references to other buffers
	 */
	void referenceIncrease() noexcept;
	void referenceDecrease() noexcept;

private:
	boost::signals2::signal<void (Update)> signalUpdate;

	bool dirty;
	std::size_t nReferences; // Used by the Kernel

	std::size_t cursor;
	friend class Kernel;
};


// Implementations

inline Buffer::Buffer() noexcept:
	dirty(false), nReferences(0), cursor(0)
{
}
inline std::string Buffer::getTitle() const noexcept
{
	return title;
}
inline bool Buffer::isDirty() const noexcept
{
	return dirty;
}
inline void Buffer::play() throw(PythonException)
{
	if (playing())
		throw PythonException{"Buffer is already playing",
		                      PythonException::Exception};
}
inline void Buffer::stop() throw(PythonException)
{
	if (!playing())
		throw PythonException{"Buffer is not playing",
		                      PythonException::Exception};
}
inline std::size_t Buffer::getCursor() const noexcept
{
	return cursor;
}
inline void Buffer::setCursor(std::size_t c) throw(PythonException)
{
	if (c >= duration())
		throw PythonException{"Cursor index is higher than duration",
		                      PythonException::IndexError};
	cursor = c;
	notifyUpdate(Update::Surface);
}
inline void Buffer::notifyUpdate(Update::Level level) noexcept
{
	signalUpdate(Update{level, IntervalIndex(0, duration())});
	dirty = true;
}
inline void Buffer::notifyUpdate(Update::Level level,
                                 IntervalIndex interval) noexcept
{
	signalUpdate(Update{level, interval});
	dirty = true;
}

inline void Buffer::referenceIncrease() noexcept
{
	++nReferences;
}
inline void Buffer::referenceDecrease() noexcept
{
	if (nReferences) --nReferences;
}

} // namespace pg

#endif // !_POLYGAMMA_CORE_BUFFER_HPP__
