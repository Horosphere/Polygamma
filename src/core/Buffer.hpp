#ifndef _POLYGAMMA_CORE_BUFFER_HPP__
#define _POLYGAMMA_CORE_BUFFER_HPP__

#include <cstdint>

#include <boost/signals2.hpp>

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
		Singular // singular/BufferSingular.hpp
	};

	/**
	 * Exposed to Python
	 */
	virtual Type getType() const noexcept = 0;

	void notifyUpdate() noexcept;

	/**
	 * @brief Registers a listener that is notified (operator()()) when
	 *	the Buffer requires a graphics update.
	 */
	template <typename Listener> void
	registerUpdateListener(Listener listener) const noexcept;
protected:
	/**
	 * Subclasses shall call this to signal a graphics update
	 */
	boost::signals2::signal<void ()> signalUpdate;
};

} // namespace pg

// Implementations

inline void pg::Buffer::notifyUpdate() noexcept
{
	signalUpdate();
}
template <typename Listener> inline void
pg::Buffer::registerUpdateListener(Listener listener) const noexcept
{
	const_cast<boost::signals2::signal<void ()>&>(signalUpdate).connect(listener);
}
#endif // !_POLYGAMMA_CORE_BUFFER_HPP__
