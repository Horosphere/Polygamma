#ifndef _POLYGAMMA_UI_EDITORS_EDITORSINGULAR_HPP__
#define _POLYGAMMA_UI_EDITORS_EDITORSINGULAR_HPP__

#include <QVBoxLayout>

#include "Editor.hpp"
#include "../graphics/Waveform.hpp"
#include "../../singular/BufferSingular.hpp"

namespace pg
{

class EditorSingular final: public Editor
{
	Q_OBJECT
public:
	explicit EditorSingular(Kernel* const kernel,
	                        BufferSingular const* const buffer,
	                        QWidget* parent = 0);

	virtual bool saveAs(QString* const error) override;
	virtual BufferSingular const* getBuffer() const noexcept override;


private Q_SLOTS:
	void onUpdateAudioFormat();
	void onSelection(Interval<long>, std::size_t index);

private:
	/**
	 * @brief Generates a callback for AxisInterval tick labeling.
	 * @return A function that when supplied with two numbers, format their
	 *  product into a timecode. The second number determines whether to show
	 *  sample index or not.
	 */
	static std::function<QString(long, long)>
	timecodeCallback(int sampleRate) noexcept;

	BufferSingular const* const buffer;

	QVBoxLayout* const layoutMain;
	Waveform** waveforms;

};

} // namespace pg

// Implementations

inline pg::BufferSingular const*
pg::EditorSingular::getBuffer() const noexcept
{
	return buffer;
}


#endif // !_POLYGAMMA_UI_EDITORS_EDITORSINGULAR_HPP__
