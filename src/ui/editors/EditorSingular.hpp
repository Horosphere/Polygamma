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
	EditorSingular(Kernel* const kernel, BufferSingular const* const buffer,
	               QWidget* parent = 0);

	virtual bool saveAs(QString* const error) override;
	virtual BufferSingular const* getBuffer() const noexcept override;


private Q_SLOTS:
	void onUpdateAudioFormat();
	void onSelection(Interval<int64_t>, std::size_t index);

private:
	BufferSingular const* const buffer;

	QVBoxLayout* const mainLayout;
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
