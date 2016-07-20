#ifndef _POLYGAMMA_UI_PANELS_PANELMULTIMEDIA_HPP__
#define _POLYGAMMA_UI_PANELS_PANELMULTIMEDIA_HPP__

#include <map>

#include <QtAV>
#include <QtAVWidgets>

#include "Panel.hpp"
#include "../../core/Buffer.hpp"

namespace pg
{

class PanelMultimedia final: public Panel
{
	Q_OBJECT
public:
	explicit PanelMultimedia(QWidget* parent = 0);
	~PanelMultimedia() noexcept; // Clean up

	void setCachingDirectory(std::string) noexcept;
	void bufferAdd(Buffer const*) noexcept;
	/*
	 * The argument does not have to be a key in the cache.
	 */
	void bufferErase(Buffer const*) noexcept;
	void bufferUpdate(Buffer const* const, IntervalIndex) noexcept;

public Q_SLOTS:
	void play(Buffer const* const);

private:
	void updateCache(Buffer const* const) noexcept;
	struct PlaybackCache
	{
		/**
		 * Equals INTERVALINDEX_NULL = [std::numeric_limits<std::size_t>::max(), 0]
		 * when sanitised.
		 * @brief A buffer region that has been updated since last caching.
		 */
		IntervalIndex dirty;
		/*
		 * TODO: Currently each buffer only corresponds to one cache file, and
		 *	updating the buffer causes the file to be outdated. Use multiple files
		 *	and only update the ones relevant to the change.
		 */
		std::string cacheFile;
	};

	std::string cachingDirectory;

	// Widgets
	QtAV::VideoOutput* const videoOutput;
	QtAV::AVPlayer* const avPlayer;

	// Dynamic
	std::string error;
	uint64_t cacheIndex; // Index of caching
	std::map<Buffer const*, PlaybackCache> caches;
};

// Implementations

inline void PanelMultimedia::setCachingDirectory(std::string dir) noexcept
{
	cachingDirectory = dir;
}
inline void PanelMultimedia::bufferAdd(Buffer const* buffer) noexcept
{
	assert(buffer);

	caches[buffer] = PlaybackCache();
	caches[buffer].dirty = IntervalIndex(0, buffer->duration());
}
inline void PanelMultimedia::bufferErase(Buffer const* buffer) noexcept
{
	auto it = caches.find(buffer);
	if (it != caches.end())
	{
		std::remove(it->second.cacheFile.c_str());
		caches.erase(it);
	}
}
inline void
PanelMultimedia::bufferUpdate(Buffer const* const buffer,
                              IntervalIndex update) noexcept
{
	caches[buffer].dirty += update;
}



} // namespace pg


#endif // !_POLYGAMMA_UI_PANELS_PANELMULTIMEDIA_HPP__
