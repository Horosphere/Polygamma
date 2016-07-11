#include "DialogPreferences.hpp"

#include <QCloseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QDebug>

#include "../ui.hpp"


#define DP_ADD_PAGE(name, displayName) \
	pageList->addItem(tr(displayName)); \
	QWidget* page##name = new QWidget;\
	QVBoxLayout* layout##name = new QVBoxLayout;\
	layout##name->setAlignment(Qt::AlignTop);\
	page##name->setLayout(layout##name)

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

	// Pages begin
	// Page::UI
	DP_ADD_PAGE(UI, "UI");

	layoutUI->addWidget(uiBG);
	layoutUI->addWidget(uiTerminalBG);

	pageStack->addWidget(pageUI);

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
#undef DP_ADD_PAGE

void pg::DialogPreferences::onReload()
{
	uiBG->onColourChanged(abgrToQColor(config->uiBG));
	uiTerminalBG->onColourChanged(abgrToQColor(config->uiTerminalBG));
}
void pg::DialogPreferences::save()
{
	config->uiBG = qColorToABGR(uiBG->getColour());
	config->uiTerminalBG = qColorToABGR(uiTerminalBG->getColour());

	config->update();
}

