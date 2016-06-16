#include "DialogPreferences.hpp"

#include <QCloseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>

#include "ui.hpp"


#define DP_ADD_PAGE(name, displayName) \
	pageList->addItem(tr(displayName)); \
	QWidget* page##name = new QWidget;\
	QVBoxLayout* layout##name = new QVBoxLayout;\
	layout##name->setAlignment(Qt::AlignTop);\
	page##name->setLayout(layout##name)

pg::DialogPreferences::DialogPreferences(Kernel* const kernel, QWidget* parent):
	QDialog(parent), kernel(kernel),
	// Various options:
	uiColourBG(new ColourButton),
	uiTerminalBG(new ColourButton)
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
	
	layoutUI->addWidget(uiColourBG);
	layoutUI->addWidget(uiTerminalBG);
	
	pageStack->addWidget(pageUI);


	// Pages end
	
	// Configure pageList so it conforms to the size of its widest child
	pageList->setMaximumWidth(pageList->sizeHintForColumn(0) + 3);

	// Setup connections
	connect(pageList, &QListWidget::currentRowChanged,
			pageStack, &QStackedWidget::setCurrentIndex);

	// Load settings from kernel
	onReload();
}
#undef DP_ADD_PAGE

void pg::DialogPreferences::closeEvent(QCloseEvent* event)
{
	save();
	QDialog::closeEvent(event);
}
void pg::DialogPreferences::onReload()
{
	uiColourBG->onColourChanged(abgrToQColor(kernel->config.uiColourBG));
	uiTerminalBG->onColourChanged(abgrToQColor(kernel->config.uiTerminalBG));
}
void pg::DialogPreferences::save()
{
	kernel->config.uiColourBG = qColorToABGR(uiColourBG->getColour());
	kernel->config.uiTerminalBG = qColorToABGR(uiTerminalBG->getColour());
}

