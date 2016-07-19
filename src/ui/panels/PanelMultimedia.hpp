#ifndef _POLYGAMMA_UI_PANELS_PANELMULTIMEDIA_HPP__
#define _POLYGAMMA_UI_PANELS_PANELMULTIMEDIA_HPP__

#include <map>

#include "Panel.hpp"
#include "../editors/Editor.hpp"

namespace pg
{

class PanelMultimedia final: public Panel
{
	Q_OBJECT
public:
	explicit PanelMultimedia(QWidget* parent = 0);

	void editorAdd(Editor*) noexcept;
	void editorErase(Editor*) noexcept;
public Q_SLOTS:
	void onPlayPause();

private:
	struct PlaybackCache
	{
		PlaybackCache() noexcept;

		bool dirty;
	};

	std::map<Editor*, PlaybackCache> caches;
};

// Implementations

inline void PanelMultimedia::editorAdd(Editor* editor) noexcept
{
	caches[editor] = PlaybackCache();
}
inline void PanelMultimedia::editorErase(Editor* editor) noexcept
{
	caches.erase(editor);
}

inline PanelMultimedia::PlaybackCache::PlaybackCache() noexcept: dirty(true)
{
}


} // namespace pg


#endif // !_POLYGAMMA_UI_PANELS_PANELMULTIMEDIA_HPP__
