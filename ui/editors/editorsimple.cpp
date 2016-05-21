#include "editorsimple.hpp"

#include <QFileDialog>
#include <QDebug>

#include "../../io/av.hpp"

pg::EditorSimple* pg::EditorSimple::fromFile(QWidget* const parent,
											 QString* const error)
{
	QString fileName = QFileDialog::getOpenFileName(parent, "Choose a file",
					   "/home", tr("Everything (*.*);; Garbage (*.cpp, *.sh)"));
	if (fileName.isNull()) return nullptr; // The user closed the window

	std::string e;
	Audio audio;
	if (!pg::readAV(fileName.toLatin1().data(), &audio, &e))
	{
		*error = tr(e.c_str());
		return nullptr;
	}
	EditorSimple* editor = new EditorSimple(parent);
	editor->audio = std::move(audio);
	editor->updateAudioFormat();
	return editor;
}

pg::EditorSimple::EditorSimple(QWidget *parent) : Editor(parent),
	mainLayout(new QVBoxLayout), waveform(nullptr)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);
	setWidget(centralWidget);
}
pg::EditorSimple::~EditorSimple()
{
	delete[] waveform;
}

void pg::EditorSimple::updateAudioFormat()
{
	// Remove all child widgets
	QLayoutItem* child;
	while ((child = mainLayout->takeAt(0)))
	{
		delete child->widget();
		delete child;
	}
	delete[] waveform;

	// Re-allocate waveform
	waveform = new Waveform*[audio.getNChannels()];
	for (std::size_t i = 0; i < audio.getNChannels(); ++i)
	{
		waveform[i] = new Waveform(this);
		waveform[i]->setChannel(&audio[i]);
		mainLayout->addWidget(waveform[i]);
	}
	for (std::size_t i = 1; i< audio.getNChannels(); ++i)
	{
		connect(waveform[0], &Waveform::rangeXChanged,
				waveform[i], &Waveform::setRangeX);
	}


}


bool pg::EditorSimple::saveAs(QString* const error)
{
	// Will write the file to disk.
	return true;
}
