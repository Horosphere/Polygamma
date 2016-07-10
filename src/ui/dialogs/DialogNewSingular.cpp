#include "DialogNewSingular.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../../core/text.hpp"

constexpr std::size_t const sampleRates[] = {44100, 72000};

pg::DialogNewSingular::DialogNewSingular(QWidget* parent): QDialog(parent),
	comboChannelLayout(new QComboBox), comboSampleRate(new QComboBox),
	lineEditDuration(new QLineEdit)
{
	QLabel* labelChannelLayout = new QLabel(tr("Channel Layout"));
	QLabel* labelSampleRate = new QLabel(tr("Sample Rate(/s)"));
	QLabel* labelDuration = new QLabel(tr("Duration(s)"));

	for (auto const& pair: channelNames)
		comboChannelLayout->addItem(QString::fromStdString(pair.second));
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
	

	QDialogButtonBox* buttonBox = new QDialogButtonBox(
			QDialogButtonBox::Ok |
			QDialogButtonBox::Cancel);
	buttonBox->setOrientation(Qt::Horizontal);
	layoutMain->addWidget(buttonBox);

	connect(buttonBox, &QDialogButtonBox::accepted,
			this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected,
			this, &QDialog::reject);
}

std::tuple<pg::ChannelLayout, std::size_t, std::string> pg::DialogNewSingular::values() const noexcept
{
	std::tuple<ChannelLayout, std::size_t, std::string> result;
	std::string channelLayoutName = comboChannelLayout->currentText().toStdString();
	for (auto const& pair: channelNames)
		if (channelLayoutName == pair.second) std::get<0>(result) = pair.first;
	std::get<1>(result) = comboSampleRate->currentText().toULong();
	std::get<2>(result) = lineEditDuration->text().toStdString();

	return result;
}

