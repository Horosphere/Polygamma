#ifndef _POLYGAMMA_UI_UTIL_COLOURBUTTON_HPP__
#define _POLYGAMMA_UI_UTIL_COLOURBUTTON_HPP__

#include <QPushButton>
#include <QColor>

namespace pg
{

/**
 * This class implements a colour picker 
 */
class ColourButton: public QPushButton
{
	Q_OBJECT

public:
	explicit ColourButton(QWidget* parent = 0);

	void setAlpha(bool isEnabled);
	
	QColor getColour() const;
Q_SIGNALS:
	/**
	 * @brief This signal is emitted whenever the user changes the colour.
	 */
	void colourChanged(QColor);

public Q_SLOTS:
	/**
	 * @brief Emission to this slot changes the current rendering colour of
	 * ColourButton. It does not trigger a colourChanged(QColor) emission.
	 */
	void onColourChanged(QColor);

private:
	void onClicked();

	QColor colour;
	bool enableAlpha;
};

} // namespace pg

// Implementations

inline void
pg::ColourButton::setAlpha(bool isEnabled)
{
	enableAlpha = isEnabled;
}

inline QColor
pg::ColourButton::getColour() const
{
	return colour;
}

#endif // !_POLYGAMMA_UI_UTIL_COLOURBUTTON_HPP__
