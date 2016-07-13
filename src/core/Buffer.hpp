#ifndef _POLYGAMMA_CORE_BUFFER_HPP__
#define _POLYGAMMA_CORE_BUFFER_HPP__

#include <cstdint>

#include <boost/signals2.hpp>

#include "python.hpp"

namespace pg
{

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
	virtual bool saveToFile(std::string fileName, std::string* const error) = 0;
	/**
	 * Exposed to Python. Wraps bool saveToFile(std::string, std::string* const)
	 *  and throws IOError upon failure.
	 */
	void saveToFile(std::string fileName) throw(PythonException);
	std::string getTitle() const noexcept;

	void notifyUpdate() noexcept;
	void uiDestroy() noexcept;

	/**
	 * @brief Registers a listener that is notified (operator()()) when
	 *  the Buffer requires a graphics update.
	 */
	template <typename Listener> void
	registerUpdateListener(Listener listener) const noexcept;
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
	boost::signals2::signal<void ()> signalUpdate;
	boost::signals2::signal<void ()> signalUIDestroy;

	std::size_t nReferences; // Used by the Kernel
	friend class Kernel;
};

} // namespace pg

// Implementations

inline pg::Buffer::Buffer() noexcept: nReferences(0)
{
}
inline void
pg::Buffer::saveToFile(std::string fileName) throw(PythonException)
{
	std::string error;
	if (!saveToFile(fileName, &error))
	{
		throw PythonException{error, PythonException::IOError};
	}
}
inline std::string pg::Buffer::getTitle() const noexcept
{
	return title;
}
inline void pg::Buffer::notifyUpdate() noexcept
{
	signalUpdate();
}
inline void pg::Buffer::uiDestroy() noexcept
{
	signalUIDestroy();
}
template <typename Listener> inline void
pg::Buffer::registerUpdateListener(Listener listener) const noexcept
{
	const_cast<boost::signals2::signal<void ()>&>(signalUpdate).connect(listener);
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
