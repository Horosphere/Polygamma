#ifndef _POLYGAMMA_UI_GRAPHICS_SPECTROGRAM_HPP__
#define _POLYGAMMA_UI_GRAPHICS_SPECTROGRAM_HPP__

#include <QPixmap>

#include "Viewport2.hpp"
#include "../../math/Audio.hpp"

namespace pg
{
class Spectrogram final: public Viewport2
{
	Q_OBJECT
public:
	explicit Spectrogram(QWidget* parent = 0);

	void setChannel(Audio::Channel* const channel,
					real const* const window,
					std::size_t windowRadius) noexcept;

protected:
	void paintEvent(QPaintEvent*);

public Q_SLOTS:
	void updateSpectrogram();

private:
	Audio::Channel* channel;
	real const* window;
	std::size_t windowRadius;

	complex** spectrogram;

};

} // namespace pg

#endif // !_POLYGAMMA_UI_GRAPHICS_SPECTROGRAM_HPP__

