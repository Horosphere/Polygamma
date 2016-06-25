#include "EditorSingular.hpp"

#include <QLineEdit>
#include <QDebug>

pg::EditorSingular::EditorSingular(Kernel* const kernel,
                                   BufferSingular* const buffer,
                                   QWidget* parent):
	Editor(kernel, parent), buffer(buffer),
	mainLayout(new QVBoxLayout), waveforms(nullptr)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);
	setWidget(centralWidget);
	this->setWindowTitle("aaa");

	onUpdateAudioFormat();
}

bool pg::EditorSingular::saveAs(QString* const error)
{
	*error = "Unsupported operation";
	return false;
}

void pg::EditorSingular::onUpdateAudioFormat()
{
	QLayoutItem* child;
	while ((child = mainLayout->takeAt(0)))
	{
		delete child->widget();
		delete child;
	}
	delete[] waveforms;

	std::size_t const nAudioChannels = buffer->nAudioChannels();
	waveforms = new Waveform*[nAudioChannels];
	for (std::size_t i = 0; i < nAudioChannels; ++i)
	{
		waveforms[i] = new Waveform(buffer, i, this);
		mainLayout->addWidget(waveforms[i]);
	}
	
}
