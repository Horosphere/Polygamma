#ifndef _POLYGAMMA_UI_EDITORS_EDITORSIMPLE_HPP__
#define _POLYGAMMA_UI_EDITORS_EDITORSIMPLE_HPP__

#include <QFile>
#include <QVBoxLayout>

#include "Editor.hpp"
#include "../graphics/Waveform.hpp"
#include "../graphics/Spectrogram.hpp"
#include "../../math/Audio.hpp"

namespace pg
{
class EditorSimple final: public Editor
{
	Q_OBJECT
public:
	static EditorSimple* fromFile(QWidget* const parent, QString* const error);

	bool saveAs(QString* const error); // To be implemented
private:
	// Constructor private so EditorSimple can be created from factory methods.
	explicit EditorSimple(QWidget *parent = 0);
	~EditorSimple();

	/**
	 * @brief updateAudioFormat Called upon the format of the field Audio audio
	 *  is changed. Currently that only happens when the number of channels is
	 *  changed.
	 */
	void updateAudioFormat();

	Audio audio;
	// UI
	QVBoxLayout* mainLayout;
	Waveform** waveform;
	Spectrogram** spectrogram;

	real* window;
	std::size_t windowRadius;

};

} // namespace pg

#endif // !_POLYGAMMA_UI_EDITORS_EDITORSIMPLE_HPP__

