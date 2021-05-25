#pragma once

#include "ggwave/ggwave.h"

#include <SDL.h>
#include <memory>

namespace ultrasonic
{
	enum class AudioType
	{
		CAPTURE,
		PLAYBACK
	};
	
	class AudioDevice
	{
	public:

		// Ctor
		// param[in]	desired_spec		The desired audio spec.
		// param[in]	audio_type			The audio type.
		AudioDevice(const SDL_AudioSpec& desired_spec, AudioType audio_type);

		// Ctor
		AudioDevice(AudioDevice&&) = default;
		
		// Dtor
		~AudioDevice();

		// Gets the audio spec.
		// return							Obtained audio spec.
		SDL_AudioSpec audio_spec() const;

		// Gets the audio sample rate.
		// return							Obtained sample rate.
		int sample_rate() const;

		// Gets the audio sample format.
		// return							Obtained audio format.
		SDL_AudioFormat format() const;

		// Pause the audio device.
		void pause();

		// Unpause the audio device.
		void unpause();

		// Gets the queued audio size.
		// return							The queued audio size. 
		uint32_t queued_audio_size();

		// Clear the queued audio.
		void clear_queued_audio();

		// Queue the audio.
		// param[in]	data				The data to queue to the device for later playback.
		// param[in]	len					The number of bytes to which (data) points.
		// return							true if successful, false otherwise.
		bool queue_audio(const void* data, uint32_t len);

		// Dequeue the audio.
		// param[in]	data				A pointer into where audio data should be copied.
		// param[in]	len					The number of bytes to which (data) points.
		// return							Number of bytes dequeued.
		uint32_t dequeue_audio(void* data, uint32_t len);
		
		// Deleted functions
		AudioDevice(const AudioDevice&) = delete;
		AudioDevice& operator=(const AudioDevice&) = delete;
		AudioDevice& operator=(AudioDevice&&) = delete;
	private:

		// Initialise the audio device.
		// param[in]	desired_spec		The desired audio spec.
		// param[in]	audio_type			The audio type.
		// return							A tuple of [audio device, obtained audio spec].
		static std::tuple<SDL_AudioDeviceID, SDL_AudioSpec> init_audio_device(const SDL_AudioSpec& desired_spec, AudioType audio_type);

		// Close the audio device.
		void close_audio();
		
		// Members
		SDL_AudioDeviceID m_device_id;
		SDL_AudioSpec m_audio_spec;
	};
	
}
