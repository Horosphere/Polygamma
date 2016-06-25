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
	EditorSingular(Kernel* const kernel, BufferSingular* const buffer,
	               QWidget* parent = 0);

	virtual bool saveAs(QString* const error) override;
	virtual BufferSingular* getBuffer() override;


private:
	void onUpdateAudioFormat();

private:
	BufferSingular* const buffer;

	QVBoxLayout* const mainLayout;
	Waveform** waveforms;

};

} // namespace pg

// Implementations

inline pg::BufferSingular*
pg::EditorSingular::getBuffer()
{
	return buffer;
}


#endif // !_POLYGAMMA_UI_EDITORS_EDITORSINGULAR_HPP__
