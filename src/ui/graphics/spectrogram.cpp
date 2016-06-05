#include "spectrogram.hpp"

#include <cmath>
#include <QPainter>

#include "../ui.hpp"
#include "../../math/fourier.hpp"

pg::Spectrogram::Spectrogram(QWidget* parent): Viewport2(parent),
	channel(nullptr), window(nullptr), windowRadius(0), spectrogram(nullptr)
{
	setDragging(true, false);
	setZoomFac(1.1, 1.0);
}

void pg::Spectrogram::setChannel(Audio::Channel * const channel,
								 real const* const window,
								 std::size_t windowRadius) noexcept
{
	this->channel = channel;
	this->window = window;
	this->windowRadius = windowRadius;
	setMaximumRange(0, ((int64_t) channel->getNSamples() - 1) * UI_SAMPLE_DISPLAY_WIDTH + 1,
					0, height());
	spectrogram = new complex*[channel->getNSamples()];
	for (std::size_t j = 0; j < channel->getNSamples(); ++j)
		spectrogram[j] = new complex[2 * windowRadius];
	maximise();
	updateSpectrogram();
}

void pg::Spectrogram::updateSpectrogram()
{
	real* integrand = new real[2 * windowRadius];
	dstft(spectrogram, channel->getData(), channel->getNSamples(),
			window, windowRadius,
			0, channel->getNSamples(), integrand);
}

void pg::Spectrogram::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	// Not drawing anything
}
