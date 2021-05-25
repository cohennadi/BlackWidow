#include "Ultrasonic.h"
#include "Defines.h"
#include "utils.h"

#include <SDL_opengl.h>
#include <chrono>


namespace ultrasonic
{

	Ultrasonic::Ultrasonic() :
		m_device_output(init_device_output()),
		m_device_input(init_device_input(m_device_output.audio_spec())),
		m_ggwave_ptr(std::make_shared<GGWave>(GGWave::Parameters{ m_device_input.sample_rate(),
																  m_device_output.sample_rate(),
																  GGWave::kDefaultSamplesPerFrame,
																  convert_to_ggwave_sample_format(m_device_input.format()),
																  convert_to_ggwave_sample_format(m_device_output.format())}))
	{}

	void Ultrasonic::run_logic()
	{
		static GGWave::CBWaveformOut cbQueueAudio = [&](const void* data, uint32_t nBytes) {
			m_device_output.queue_audio(data, nBytes);
		};

		static GGWave::CBWaveformInp cbWaveformInput = [&](void* data, uint32_t nMaxBytes) {
			return m_device_input.dequeue_audio(data, nMaxBytes);
		};

		if (m_ggwave_ptr->hasTxData() == false) {
			m_device_output.unpause();

			static auto tLastNoData = std::chrono::high_resolution_clock::now();
			auto tNow = std::chrono::high_resolution_clock::now();

			if (m_device_output.queued_audio_size() < m_ggwave_ptr->getSamplesPerFrame() * m_ggwave_ptr->getSampleSizeBytesOut()) {
				m_device_input.unpause();
				
				if (getTime_ms(tLastNoData, tNow) > 500.0f) {
					m_ggwave_ptr->decode(cbWaveformInput);
					if (m_device_input.queued_audio_size() > 32 * m_ggwave_ptr->getSamplesPerFrame() * m_ggwave_ptr->getSampleSizeBytesInp()) {
						m_device_input.clear_queued_audio();
					}
				}
				else {
					m_device_input.clear_queued_audio();
				}
			}
			else {
				tLastNoData = tNow;
			}
		}
		else {
			m_device_output.pause();
			m_device_input.pause();

			m_ggwave_ptr->encode(cbQueueAudio);
		}
	}

	std::shared_ptr<GGWave> Ultrasonic::get_ggwave()
	{
		return m_ggwave_ptr;
	}


	AudioDevice Ultrasonic::init_device_input(const SDL_AudioSpec& output_device_spec)
	{
		SDL_AudioSpec capture_spec = output_device_spec;
		capture_spec.freq = GGWave::kBaseSampleRate;
		capture_spec.format = AUDIO_F32SYS;
		capture_spec.samples = 4096;

		return AudioDevice(capture_spec, AudioType::CAPTURE);
	}

	AudioDevice Ultrasonic::init_device_output()
	{
		SDL_AudioSpec playback_spec;
		SDL_zero(playback_spec);

		playback_spec.freq = GGWave::kBaseSampleRate;
		playback_spec.format = AUDIO_S16SYS;
		playback_spec.channels = 1;
		playback_spec.samples = 16 * 1024;
		playback_spec.callback = NULL;

		return AudioDevice(playback_spec, AudioType::PLAYBACK);
	}

	ggwave_SampleFormat Ultrasonic::convert_to_ggwave_sample_format(SDL_AudioFormat sdl_format)
	{
		switch (sdl_format) {
		case AUDIO_U8:      return GGWAVE_SAMPLE_FORMAT_U8;
		case AUDIO_S8:      return GGWAVE_SAMPLE_FORMAT_I8;
		case AUDIO_U16SYS:  return GGWAVE_SAMPLE_FORMAT_U16;
		case AUDIO_S16SYS:  return GGWAVE_SAMPLE_FORMAT_I16;
		case AUDIO_S32SYS:  return GGWAVE_SAMPLE_FORMAT_F32;
		case AUDIO_F32SYS:  return  GGWAVE_SAMPLE_FORMAT_F32;
		}
	}
}
