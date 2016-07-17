#include "DialogPreferences.hpp"

#include <QAudioDeviceInfo>
#include <QCloseEvent>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDebug>

#include "../ui.hpp"


pg::DialogPreferences::DialogPreferences(Configuration* const config,
    QWidget* parent):
	QDialog(parent), config(config),
	// Various options:
	uiBG(new ColourButton), uiTerminalBG(new ColourButton)
{
	QHBoxLayout* layoutMain = new QHBoxLayout;
	setLayout(layoutMain);

	QListWidget* pageList = new QListWidget;
	layoutMain->addWidget(pageList);
	QStackedWidget* pageStack = new QStackedWidget;
	layoutMain->addWidget(pageStack);

#define DP_ADD_PAGE(name, displayName) \
	pageList->addItem(tr(displayName)); \
	QWidget* page##name = new QWidget;\
	QVBoxLayout* layout##name = new QVBoxLayout;\
	layout##name->setAlignment(Qt::AlignTop);\
	page##name->setLayout(layout##name)

	// Pages begin
	/*
	DP_ADD_PAGE(IO, "Devices");

	layoutIO->addWidget(ioAudioDeviceInput);
	layoutIO->addWidget(ioAudioDeviceOutput);

	pageStack->addWidget(pageIO);
	*/

	// Page::UI
	DP_ADD_PAGE(UI, "Appearance");

	layoutUI->addWidget(uiBG);
	layoutUI->addWidget(uiTerminalBG);

	pageStack->addWidget(pageUI);

#undef DP_ADD_PAGE
	// Pages end

	// Configure pageList so it conforms to the size of its widest child
	pageList->setMaximumWidth(pageList->sizeHintForColumn(0) + 3);

	connect(pageList, &QListWidget::currentRowChanged,
	        pageStack, &QStackedWidget::setCurrentIndex);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(
	  QDialogButtonBox::Apply |
	  QDialogButtonBox::Close);
	buttonBox->setOrientation(Qt::Vertical);
	layoutMain->addWidget(buttonBox);

	connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
	        this, &DialogPreferences::save);
	connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked,
	        this, &DialogPreferences::hide);

	// Load settings from kernel
	onReload();
}

void pg::DialogPreferences::onReload()
{
	/*
	ioAudioDeviceInput->clear();
	QList<QAudioDeviceInfo> availableInputDevices =
		QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	for (std::size_t i = 0; i < availableInputDevices.size(); ++i)
	{
		QString deviceName = availableInputDevices[i].deviceName();
		ioAudioDeviceInput->addItem(deviceName);
		if (deviceName == QString::fromStdString(config->ioAudioDeviceInput))
			ioAudioDeviceInput->setCurrentIndex(i);
	}
	ioAudioDeviceOutput->clear();
	QList<QAudioDeviceInfo> availableOutputDevices =
		QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (std::size_t i = 0; i < availableOutputDevices.size(); ++i)
	{
		QString deviceName = availableOutputDevices[i].deviceName();
		ioAudioDeviceOutput->addItem(deviceName);
		if (deviceName == QString::fromStdString(config->ioAudioDeviceOutput))
			ioAudioDeviceOutput->setCurrentIndex(i);
	}
	*/

	uiBG->onColourChanged(abgrToQColor(config->uiBG));
	uiTerminalBG->onColourChanged(abgrToQColor(config->uiTerminalBG));
}
void pg::DialogPreferences::save()
{
	//config->ioAudioDeviceInput = ioAudioDeviceInput->currentText().toStdString();
	//config->ioAudioDeviceOutput = ioAudioDeviceOutput->currentText().toStdString();
	config->uiBG = qColorToABGR(uiBG->getColour());
	config->uiTerminalBG = qColorToABGR(uiTerminalBG->getColour());

	config->update();
}

