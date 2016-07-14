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

#endif // !_POLYGAMMA_UI_MULTIMEDIAENGINE_HPP__
