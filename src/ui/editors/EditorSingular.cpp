#include "EditorSingular.hpp"

#include <string>

#include <QLineEdit>
#include <QDebug>

#include "../util/Axis.hpp"


pg::EditorSingular::EditorSingular(Kernel* const kernel,
                                   BufferSingular const* const buffer,
                                   QWidget* parent):
	Editor(kernel, buffer, parent), buffer(buffer),
	mainLayout(new QVBoxLayout), waveforms(nullptr)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);
	setWidget(centralWidget);

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

	AxisInterval* axis = new AxisInterval(this);
	mainLayout->addWidget(axis);

	std::size_t const nAudioChannels = buffer->nAudioChannels();
	waveforms = new Waveform*[nAudioChannels];
	for (std::size_t i = 0; i < nAudioChannels; ++i)
	{
		waveforms[i] = new Waveform(buffer, i, this);
		connect(waveforms[i], &Waveform::selectionX,
		        this, [this, i](Interval<long> selection)
		{
			this->onSelection(selection, i);
		});
		mainLayout->addWidget(waveforms[i]);
	}
	connect(waveforms[0], &Viewport2::rangeXChanged,
			axis, &AxisInterval::onIntervalChanged);
}

void pg::EditorSingular::onSelection(Interval<long> selection,
		std::size_t index)
{
	std::string string = std::string(PYTHON_KERNEL) + ".buffers[" +
		std::to_string(kernel->bufferIndex(buffer)) + "].select(" +
		std::to_string(index) + ", " + std::to_string(selection.begin) +
		", " + std::to_string(selection.end) + ')';
	Q_EMIT execute(Script(string));

}
