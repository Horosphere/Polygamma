#ifndef EDITORSIMPLE_HPP
#define EDITORSIMPLE_HPP

#include <QFile>
#include <QVBoxLayout>

#include "editor.hpp"
#include "../graphics/waveform.hpp"
#include "../graphics/spectrogram.hpp"
#include "../../math/audio.hpp"

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

#endif // EDITORSIMPLE_HPP
