#include "DialogNewSingular.hpp"

#include <QVBoxLayout>
#include <QDialogButtonBox>

constexpr std::size_t const sampleRates[] = {44100, 72000};

pg::DialogNewSingular::DialogNewSingular(QWidget* parent): QDialog(parent),
	comboChannelLayout(new QComboBox), comboSampleRate(new QComboBox),
	lineEditDuration(new QLineEdit)
{
	for (auto const& pair: channelNames)
		comboChannelLayout->addItem(QString::fromStdString(pair.second));
	for (std::size_t const sampleRate: sampleRates)
		comboSampleRate->addItem(QString::number(sampleRate));

	QVBoxLayout* layoutMain = new QVBoxLayout;
	setLayout(layoutMain);

	layoutMain->addWidget(comboChannelLayout);
	layoutMain->addWidget(comboSampleRate);
	layoutMain->addWidget(lineEditDuration);

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

