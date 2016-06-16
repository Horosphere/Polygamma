#include "ColourButton.hpp"

#include <QColorDialog>

pg::ColourButton::ColourButton(QWidget* parent): QPushButton(parent),
	enableAlpha(false)	
{
	setFixedSize(50, 20);
	connect(this, &QPushButton::clicked,
	        this, &ColourButton::onClicked);
}

void pg::ColourButton::onColourChanged(QColor c)
{
	if (c.isValid())
	{
		QString styleSheet = QString("background-color: %1").arg(c.name());
		setStyleSheet(styleSheet);
		colour = c;
	}
}
void pg::ColourButton::onClicked()
{
	QColor c;
	if (enableAlpha)
		// The default title is "Select Color"
		c = QColorDialog::getColor(colour, this, tr("Select Color"),
				QColorDialog::ShowAlphaChannel);
	else
		c = QColorDialog::getColor(colour, this);
	if (c.isValid())
	{
		QString styleSheet = QString("background-color: %1").arg(c.name());
		setStyleSheet(styleSheet);
		colour = c;
		Q_EMIT colourChanged(c);
	}
}
