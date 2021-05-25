#pragma once
#include "AudioDevice.h"
#include "SdlWrapper.h"

#include <ggwave/ggwave.h>
#include <memory>


namespace ultrasonic
{
	class Ultrasonic
	{
	public:

		// Ctor
		Ultrasonic();

		// Dtor
		~Ultrasonic() = default;

		// Runs the ultrasonic logic.
		void run_logic();

		// Gets the ggwave instance.
		// return									The ggwave.
		std::shared_ptr<GGWave> get_ggwave();
		
		// Deleted functions
		Ultrasonic(const Ultrasonic&) = delete;
		Ultrasonic(Ultrasonic&&) = delete;
		Ultrasonic& operator=(const Ultrasonic&) = delete;
		Ultrasonic& operator=(Ultrasonic&&) = delete;
	private:

		// initialise device input.
		// param[in]	output_device_spec			The output device spec.
		// return									The audio device.
		static AudioDevice init_device_input(const SDL_AudioSpec& output_device_spec);

		// initialise device output.
		// return									The audio device.
		static AudioDevice init_device_output();

		// Converts to ggwave sample format.
		// param[in]	sdl_format					The sdl format to convert.
		// return									The ggwave format.
		static ggwave_SampleFormat convert_to_ggwave_sample_format(SDL_AudioFormat sdl_format);

		// Members
		SdlWrapper m_sdl;
		AudioDevice m_device_output;
		AudioDevice m_device_input;
		std::shared_ptr<GGWave> m_ggwave_ptr;
		
	};
}
