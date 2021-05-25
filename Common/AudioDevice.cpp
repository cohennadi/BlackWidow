#pragma once

#include "AudioDevice.h"
#include "Defines.h"

#include <cassert>

using namespace ultrasonic;

AudioDevice::AudioDevice(const SDL_AudioSpec& desired_spec, AudioType audio_type)
{
	auto [device_id, audio_spec] = init_audio_device(desired_spec, audio_type);
	m_device_id = device_id;
	m_audio_spec = audio_spec;
}

AudioDevice::~AudioDevice()
{
	try {
		pause();
		close_audio();
	} catch(...) {
		LOG("Error closing audio");
		assert(true);
	}
}

SDL_AudioSpec AudioDevice::audio_spec() const
{
	return m_audio_spec;
}

int AudioDevice::sample_rate() const
{
	return m_audio_spec.freq;
}

SDL_AudioFormat AudioDevice::format() const
{
	return m_audio_spec.format;
}

void AudioDevice::pause()
{
	SDL_PauseAudioDevice(m_device_id, SDL_TRUE);
}

void AudioDevice::unpause()
{
	SDL_PauseAudioDevice(m_device_id, SDL_FALSE);
}

uint32_t AudioDevice::queued_audio_size()
{
	return SDL_GetQueuedAudioSize(m_device_id);
}

void AudioDevice::clear_queued_audio()
{
	SDL_ClearQueuedAudio(m_device_id);
}

bool AudioDevice::queue_audio(const void* data, uint32_t len)
{
	return SDL_QueueAudio(m_device_id, data, len) == 0;
}

uint32_t AudioDevice::dequeue_audio(void* data, uint32_t len)
{
	return SDL_DequeueAudio(m_device_id, data, len);
}

std::tuple<SDL_AudioDeviceID, SDL_AudioSpec> AudioDevice::init_audio_device(const SDL_AudioSpec& desired_spec, 
                                                                            const AudioType audio_type)
{
	SDL_AudioSpec obtained_spec;
	SDL_zero(obtained_spec);

	constexpr char* DEFAULT_DEVICE = nullptr;
	const int is_capture = (audio_type == AudioType::CAPTURE) ? SDL_TRUE: SDL_FALSE;
	constexpr int NO_CHANGES = 0;
	const SDL_AudioDeviceID input_device_id = SDL_OpenAudioDevice(DEFAULT_DEVICE, is_capture, &desired_spec, &obtained_spec, NO_CHANGES);

	if (!input_device_id) {
		const std::string log_message = std::string("Couldn't open an audio device: ") + SDL_GetError();
		LOG(log_message);
		//throw; // TODO: add exceptions class
	}

	return { input_device_id, obtained_spec };
}

void AudioDevice::close_audio()
{
	SDL_CloseAudioDevice(m_device_id);
}