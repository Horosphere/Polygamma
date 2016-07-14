#ifndef _POLYGAMMA_UI_MULTIMEDIAENGINE_HPP__
#define _POLYGAMMA_UI_MULTIMEDIAENGINE_HPP__

#include <map>

#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QBuffer>

#include "editors/Editor.hpp"
#include "../core/Configuration.hpp"

namespace pg
{

class MultimediaEngine final
{
public:
	MultimediaEngine(QObject* parent = 0);

	void loadConfiguration(Configuration const* const config);

	void addBuffer(Editor*) noexcept;
	void eraseBuffer(Editor*) noexcept;

	/**
	 * addBuffer(Editor*) must be called before this
	 */
	void startPlayback(Editor*) noexcept;
	
private:
	/**
	 * @brief As this class is not a QObject, the parent will only be used to
	 *	create other QObjects
	 */
	QObject* parent;

	QAudioDeviceInfo audioDeviceInput;

	QAudioDeviceInfo audioDeviceOutput;
	QBuffer audioBufferOutput;

	struct PlaybackCache
	{
		PlaybackCache() noexcept: dirty(true), audioOutput(nullptr) {}

		// TODO: Use a bitset so the entire buffer doesn't have to be updated
		// everytime there is an update
		bool dirty;
		QAudioFormat audioFormat;
		QByteArray audioSamples;
		QAudioOutput* audioOutput;
	};
	std::map<Editor*, PlaybackCache> caches;
};

} // namespace pg


// Implementations

inline pg::MultimediaEngine::MultimediaEngine(QObject* parent): parent(parent)
{
}
inline void pg::MultimediaEngine::addBuffer(Editor* buffer) noexcept
{
	caches[buffer] = PlaybackCache();
}
inline void pg::MultimediaEngine::eraseBuffer(Editor* buffer) noexcept
{
	caches.erase(buffer);
}
#endif // !_POLYGAMMA_UI_MULTIMEDIAENGINE_HPP__
