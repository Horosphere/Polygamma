#include "DialogNewSingular.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../../core/text.hpp"

constexpr std::size_t const sampleRates[] = {44100, 72000};

pg::DialogNewSingular::DialogNewSingular(QWidget* parent):
	DialogScriptArgs(parent),

	comboChannelLayout(new QComboBox), comboSampleRate(new QComboBox),
	lineEditDuration(new QLineEdit)
{
	QLabel* labelChannelLayout = new QLabel(tr("Channel Layout"));
	QLabel* labelSampleRate = new QLabel(tr("Sample Rate(/s)"));
	QLabel* labelDuration = new QLabel(tr("Duration(s)"));

	for (auto const& channelName: channelNames)
		comboChannelLayout->addItem(QString::fromStdString(boost::get<1>(channelName)));
	for (std::size_t const sampleRate: sampleRates)
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

QString pg::DialogNewSingular::script()
{
	QString result = QString(PYTHON_KERNEL) + ".createSingular(";
	std::string channelLayoutName = comboChannelLayout->currentText().toStdString();
	for (auto const& channelName: channelNames)
		if (channelLayoutName == boost::get<1>(channelName)) 
		{
			result += "pg." + QString::fromStdString(boost::get<2>(channelName)) + ", ";
			break;
		}
	result += comboSampleRate->currentText() + ", \"" +
		lineEditDuration->text() + "\")";
	return result;
}

