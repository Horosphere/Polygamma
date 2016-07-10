#ifndef _POLYGAMMA_CORE_BUFFER_HPP__
#define _POLYGAMMA_CORE_BUFFER_HPP__

#include <cstdint>

#include <boost/signals2.hpp>

#include "python.hpp"

namespace pg
{

/**
 * This class uses a case pattern. E.g. if getType() == Singular, the instance
 * must be an instance of BufferSingular.
 * @brief Each instance of Buffer corresponds to a file or editable content.
 */
class Buffer
{
public:
	virtual ~Buffer();
	enum Type
	{
		Singular = 1 << 0 // singular/BufferSingular.hpp
	};

	/**
	 * Exposed to Python
	 */
	virtual Type getType() const noexcept = 0;
	virtual bool saveToFile(std::string fileName, std::string* const error) = 0;
	/**
	 * Exposed to Python. Wraps bool saveToFile(std::string, std::string* const)
	 *	and throws IOError upon failure.
	 */
	void saveToFile(std::string fileName) throw(PythonException);

	void notifyUpdate() noexcept;
	void destroy() noexcept;

	/**
	 * @brief Registers a listener that is notified (operator()()) when
	 *	the Buffer requires a graphics update.
	 */
	template <typename Listener> void
	registerUpdateListener(Listener listener) const noexcept;
	template <typename Listener> void
	registerDestroyListener(Listener listener) const noexcept;
protected:
	/**
	 * Subclasses shall call this to signal a graphics update
	 */
	boost::signals2::signal<void ()> signalUpdate;

private:
	boost::signals2::signal<void ()> signalDestroy;
};

} // namespace pg

// Implementations

inline void
pg::Buffer::saveToFile(std::string fileName) throw(PythonException)
{
	std::string error;
	if (!saveToFile(fileName, &error))
	{
		throw PythonException{error, PythonException::IOError};
	}
}

inline void pg::Buffer::notifyUpdate() noexcept
{
	signalUpdate();
}
inline void pg::Buffer::destroy() noexcept
{
	signalDestroy();
}
template <typename Listener> inline void
pg::Buffer::registerUpdateListener(Listener listener) const noexcept
{
	const_cast<boost::signals2::signal<void ()>&>(signalUpdate).connect(listener);
}
template <typename Listener> inline void
pg::Buffer::registerDestroyListener(Listener listener) const noexcept
{
	const_cast<boost::signals2::signal<void ()>&>(signalDestroy).connect(listener);
}
#endif // !_POLYGAMMA_CORE_BUFFER_HPP__
