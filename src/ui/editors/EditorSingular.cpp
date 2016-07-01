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
	qDebug() << buffer->sampleRate;
	qDebug() << "Test" << timecodeCallback(buffer->sampleRate)(22055 + 44100 * 50 + 44100 * 60 * 2, 1);
	axis->setIntervalLabelingFunction(timecodeCallback(buffer->sampleRate));
	axis->interval.begin = 0;
	axis->interval.end = buffer->nAudioSamples();
	mainLayout->addWidget(axis);

	std::size_t const nAudioChannels = buffer->nAudioChannels();
	waveforms = new Waveform*[nAudioChannels];
	for (std::size_t i = 0; i < nAudioChannels; ++i)
	{
		waveforms[i] = new Waveform(buffer, i, this);
		waveforms[i]->setAxisX(axis);
		connect(waveforms[i], &Waveform::selectionX,
		        this, [this, i](Interval<long> selection)
		{
			this->onSelection(selection, i);
		});
		mainLayout->addWidget(waveforms[i]);

		connect(waveforms[i], &Viewport2::rangeXChanged,
				axis, &AxisInterval::onIntervalChanged);
		connect(axis, &AxisInterval::recalculationComplete,
				waveforms[i], &Viewport2::updateFromAxes);
		waveforms[i]->updateFromAxes();
	}
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

std::function<QString (long, long)>
pg::EditorSingular::timecodeCallback(int sampleRate) noexcept
{
	return [sampleRate](long base, long mod) -> QString
	{
		QString result;
		if (mod % sampleRate == 0)
		{
			base *= mod / sampleRate; // base is in seconds
			result = QString::number(base % 60).rightJustified(2, '0'); // Seconds
			base /= 60; // base is in minutes
			if (base) result.prepend(QString::number(base % 60).rightJustified(2, '0') + ':');
			base /= 60; // base is in hours
			if (base) result.prepend(QString::number(base) + ':');
		}
		else
		{
			base *= mod; // base is in samples
			result = QString::number(base % sampleRate).rightJustified(5, '0');
			base /= sampleRate; // base is in seconds
			result.prepend(QString::number(base % 60).rightJustified(2, '0') + ':');
			base /= 60; // base is in minutes
			if (base) result.prepend(QString::number(base % 60).rightJustified(2, '0') + ':');
			base /= 60; // base is in hours
			if (base) result.prepend(QString::number(base) + ':');
		}
		return result;
	};
}

