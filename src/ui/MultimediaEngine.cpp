#include "MultimediaEngine.hpp"

#include "../singular/BufferSingular.hpp"

void pg::MultimediaEngine::loadConfiguration(Configuration const* const config)
{
	audioDeviceInput = QAudioDeviceInfo::defaultInputDevice();
	QString audioDeviceInputName =
	  QString::fromStdString(config->ioAudioDeviceInput);
	for (auto const& device: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
		if (device.deviceName() == audioDeviceInputName)
		{
			audioDeviceInput = device;
			break;
		}
	audioDeviceOutput = QAudioDeviceInfo::defaultOutputDevice();
	QString audioDeviceOutputName =
	  QString::fromStdString(config->ioAudioDeviceOutput);
	for (auto const& device: QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
		if (device.deviceName() == audioDeviceOutputName)
		{
			audioDeviceOutput = device;
			break;
		}
}

void pg::MultimediaEngine::startPlayback(Editor* editor) noexcept
{

	PlaybackCache* const cache = &caches[editor];
	if (cache->dirty)
	{
		delete cache->audioOutput;
		// Switch on editor type
		if (auto const* buffer =
		      dynamic_cast<BufferSingular const*>(editor->getBuffer()))
		{
			cache->audioFormat.setSampleRate(buffer->timeBase());
			cache->audioFormat.setChannelCount(buffer->nAudioChannels());

			// Must use 32 bit floating point since 64 bit ones don't work.
			typedef float Value;
			cache->audioFormat.setSampleType(QAudioFormat::Float);
			cache->audioFormat.setSampleSize(8 * sizeof(Value));

			// Resize the audioSamples buffer to fit the samples
			std::size_t nSamples = buffer->duration();
			std::size_t nChannels = buffer->nAudioChannels();
			cache->audioSamples.resize(cache->audioFormat.sampleSize() / 8 *
			                           cache->audioFormat.channelCount() * nSamples);
			// Copy audio samples into cache->audioSamples
			Value* data = reinterpret_cast<Value*>(cache->audioSamples.data());
			for (std::size_t i = 0; i < nSamples; ++i)
			{
				for (std::size_t channel = 0; channel < nChannels; ++channel)
				{
					*data = (float) buffer->audioChannel(channel)->operator[](i);
					++data;
				}
			}

		}
		else
			assert(false && "Buffer type unrecognised by MultimediaEngine");

		// Initialise new audioOutput
		cache->audioOutput = new QAudioOutput(audioDeviceOutput,
		                                     cache->audioFormat, parent);

		// TODO: Utilise notification intervals to move the cursor
		cache->audioOutput->setNotifyInterval(100);

		cache->dirty = false;
	}

	audioBufferOutput.close();
	audioBufferOutput.setBuffer(&cache->audioSamples);
	audioBufferOutput.open(QIODevice::ReadOnly);

	cache->audioOutput->start(&audioBufferOutput);
}
