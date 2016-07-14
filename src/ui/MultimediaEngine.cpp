#include "MultimediaEngine.hpp"

pg::MultimediaEngine::MultimediaEngine()
{
}
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
