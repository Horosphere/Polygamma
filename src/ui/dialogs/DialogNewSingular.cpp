#include "DialogNewSingular.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../../core/text.hpp"

namespace pg
{

DialogNewSingular::DialogNewSingular(QWidget* parent):
	DialogScriptArgs(parent),

	comboChannelLayout(new QComboBox), comboSampleRate(new QComboBox),
	lineEditDuration(new QLineEdit)
{
	QLabel* labelChannelLayout = new QLabel(tr("Channel Layout"));
	QLabel* labelSampleRate = new QLabel(tr("Sample Rate(/s)"));
	QLabel* labelDuration = new QLabel(tr("Duration(s)"));

	for (auto const& channelName: channelNames)
	{
		int nChannels = av_get_channel_layout_nb_channels(boost::get<0>(channelName));
		comboChannelLayout->addItem('[' + QString::number(nChannels) + "] " +
		                            QString::fromStdString(boost::get<1>(channelName)));
	}
	for (std::size_t const sampleRate: SAMPLE_RATES)
		comboSampleRate->addItem(QString::number(sampleRate));

	QVBoxLayout* layoutMain = new QVBoxLayout;
	setLayout(layoutMain);

	QHBoxLayout* layoutChannelLayout = new QHBoxLayout;
	layoutChannelLayout->addWidget(labelChannelLayout);
	layoutChannelLayout->addWidget(comboChannelLayout);
	layoutMain->addLayout(layoutChannelLayout);
	QHBoxLayout* layoutSampleRate = new QHBoxLayout;
	layoutSampleRate->addWidget(labelSampleRate);
	layoutSampleRate->addWidget(comboSampleRate);
	layoutMain->addLayout(layoutSampleRate);
	QHBoxLayout* layoutDuration = new QHBoxLayout;
	layoutDuration->addWidget(labelDuration);
	layoutDuration->addWidget(lineEditDuration);
	layoutMain->addLayout(layoutDuration);
	layoutMain->addWidget(buttonBox);
}

QString DialogNewSingular::script()
{
	auto channelName = channelNames[comboChannelLayout->currentIndex()];

	return QString(PYTHON_KERNEL) + ".createSingular(pg." +
	       QString::fromStdString(boost::get<2>(channelName)) + ", " +
	       comboSampleRate->currentText() + ", \"" +
	       lineEditDuration->text() + "\")";
}


} // namespace pg
