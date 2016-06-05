#ifndef SPECTROGRAM_HPP
#define SPECTROGRAM_HPP

#include <QPixmap>

#include "viewport2.hpp"
#include "../../math/audio.hpp"

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

#endif // SPECTROGRAM_HPP
