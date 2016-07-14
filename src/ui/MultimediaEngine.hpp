#ifndef _POLYGAMMA_UI_MULTIMEDIAENGINE_HPP__
#define _POLYGAMMA_UI_MULTIMEDIAENGINE_HPP__

#include <map>

#include <QAudioDeviceInfo>

#include "editors/Editor.hpp"
#include "../core/Configuration.hpp"

namespace pg
{

class MultimediaEngine final
{
public:
	MultimediaEngine();

	void loadConfiguration(Configuration const* const config);

	void addBuffer(Editor*) noexcept;
	void eraseBuffer(Editor*) noexcept;
private:
	QAudioDeviceInfo audioDeviceInput;
	QAudioDeviceInfo audioDeviceOutput;

	struct PlaybackCache
	{
		QAudioFormat format;
		QByteArray buffer;
	};
	std::map<Editor*, PlaybackCache> caches;
};

} // namespace pg


// Implementations

inline void pg::MultimediaEngine::addBuffer(Editor* buffer) noexcept
{
	caches[buffer] = PlaybackCache();
}
inline void pg::MultimediaEngine::eraseBuffer(Editor* buffer) noexcept
{
	caches.erase(buffer);
}
#endif // !_POLYGAMMA_UI_MULTIMEDIAENGINE_HPP__
