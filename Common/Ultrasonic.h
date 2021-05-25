#pragma once

#include "ggwave/ggwave.h"

#include <SDL.h>
#include <memory>

namespace ultrasonic
{
	class Ultrasonic
	{
	public:

		// Ctor
		Ultrasonic();

		// Dtor
		~Ultrasonic();

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

		// initialise sdl.
		static void init_sdl();

		// initialise device input.
		// param[out]	o_sample_format				The input device sample format.
		// param[out]	o_sample_rate				The input device sample rate.
		// param[in]	output_device_spec			The output device spec.
		// return									The audio device id
		static SDL_AudioDeviceID init_device_input(GGWave::SampleFormat &o_sample_format, int& o_sample_rate, const SDL_AudioSpec& output_device_spec);

		// initialise device output.
		// param[out]	o_sample_format				The output device sample format.
		// param[out]	o_sample_rate				The output device sample rate.
		// param[out]	o_output_device_spec		The output device spec.
		// return									The audio device id
		static SDL_AudioDeviceID init_device_output(GGWave::SampleFormat &o_sample_format, int& o_sample_rate, SDL_AudioSpec& o_output_device_spec);

		// Members
		SDL_AudioDeviceID m_device_id_input;
		SDL_AudioDeviceID m_device_id_output;
		std::shared_ptr<GGWave> m_ggwave_ptr;
		
	};
}
