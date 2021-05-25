#include "Ultrasonic.h"
#include "Defines.h"

#include <SDL_opengl.h>
#include <chrono>

#include "ggwave-common.h"

namespace ultrasonic
{

Ultrasonic::Ultrasonic()
{
	init_sdl();

	int sample_rate_output = 0;
	GGWave::SampleFormat output_sample_format = GGWAVE_SAMPLE_FORMAT_UNDEFINED;
	SDL_AudioSpec audio_spec;
	m_device_id_output = init_device_output(output_sample_format, sample_rate_output, audio_spec);
	
	int sample_rate_input = 0;
	GGWave::SampleFormat input_sample_format = GGWAVE_SAMPLE_FORMAT_UNDEFINED;
	m_device_id_input = init_device_input(input_sample_format, sample_rate_input, audio_spec);

	m_ggwave_ptr = std::make_shared<GGWave>(GGWave::Parameters{sample_rate_input,
															   sample_rate_output,
															   GGWave::kDefaultSamplesPerFrame,
															   input_sample_format,
															   output_sample_format});
}

Ultrasonic::~Ultrasonic()
{
	SDL_PauseAudioDevice(m_device_id_input, 1);
	SDL_CloseAudioDevice(m_device_id_input);
	SDL_PauseAudioDevice(m_device_id_output, 1);
	SDL_CloseAudioDevice(m_device_id_output);
	SDL_CloseAudio();
	SDL_Quit();
}

void Ultrasonic::run_logic()
{
	static GGWave::CBWaveformOut cbQueueAudio = [&](const void* data, uint32_t nBytes) {
		SDL_QueueAudio(m_device_id_output, data, nBytes);
	};

	static GGWave::CBWaveformInp cbWaveformInput = [&](void* data, uint32_t nMaxBytes) {
		return SDL_DequeueAudio(m_device_id_input, data, nMaxBytes);
	};

	if (m_ggwave_ptr->hasTxData() == false) {
		SDL_PauseAudioDevice(m_device_id_output, SDL_FALSE);

		static auto tLastNoData = std::chrono::high_resolution_clock::now();
		auto tNow = std::chrono::high_resolution_clock::now();

		if ((int)SDL_GetQueuedAudioSize(m_device_id_output) < m_ggwave_ptr->getSamplesPerFrame() * m_ggwave_ptr->getSampleSizeBytesOut()) {
			SDL_PauseAudioDevice(m_device_id_input, SDL_FALSE);
			if (getTime_ms(tLastNoData, tNow) > 500.0f) {
				m_ggwave_ptr->decode(cbWaveformInput);
				if ((int)SDL_GetQueuedAudioSize(m_device_id_input) > 32 * m_ggwave_ptr->getSamplesPerFrame() * m_ggwave_ptr->getSampleSizeBytesInp()) {
					SDL_ClearQueuedAudio(m_device_id_input);
				}
			}
			else {
				SDL_ClearQueuedAudio(m_device_id_input);
			}
		}
		else {
			tLastNoData = tNow;
		}
	}
	else {
		SDL_PauseAudioDevice(m_device_id_output, SDL_TRUE);
		SDL_PauseAudioDevice(m_device_id_input, SDL_TRUE);

		m_ggwave_ptr->encode(cbQueueAudio);
	}
}

std::shared_ptr<GGWave> Ultrasonic::get_ggwave()
{
	return m_ggwave_ptr;
}

void Ultrasonic::init_sdl()
{
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		const std::string log_message = std::string("Couldn't initialize SDL: ") + SDL_GetError();
		LOG(log_message);
		
		return;
	}

	if (SDL_SetHintWithPriority(SDL_HINT_AUDIO_RESAMPLING_MODE, "medium", SDL_HINT_OVERRIDE) != SDL_TRUE)
	{
		const std::string log_message = std::string("Couldn't set priority SDL: ") + SDL_GetError();
		LOG(log_message);
	}
}

SDL_AudioDeviceID Ultrasonic::init_device_input(GGWave::SampleFormat& o_sample_format, int& o_sample_rate, const SDL_AudioSpec& output_device_spec)
{
	SDL_AudioSpec captureSpec = output_device_spec;
	captureSpec.freq = GGWave::kBaseSampleRate;
	captureSpec.format = AUDIO_F32SYS;
	captureSpec.samples = 4096;

	SDL_AudioSpec obtained_spec;
	SDL_zero(obtained_spec);

	constexpr char* DEFAULT_DEVICE = nullptr;
	constexpr int CAPTURE_AUDIO = SDL_TRUE;
	constexpr int NO_CHANGES = 0;
	SDL_AudioDeviceID input_device_id = SDL_OpenAudioDevice(DEFAULT_DEVICE, CAPTURE_AUDIO, &captureSpec, &obtained_spec, NO_CHANGES);

	if (!input_device_id) {
		const std::string log_message = std::string("Couldn't open an audio device for capture: ") + SDL_GetError();
		LOG(log_message);
		input_device_id = 0;
	}

	o_sample_rate = obtained_spec.freq;

	switch (obtained_spec.format) {
		case AUDIO_U8:      o_sample_format = GGWAVE_SAMPLE_FORMAT_U8;  break;
		case AUDIO_S8:      o_sample_format = GGWAVE_SAMPLE_FORMAT_I8;  break;
		case AUDIO_U16SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_U16; break;
		case AUDIO_S16SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_I16; break;
		case AUDIO_S32SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_F32; break;
		case AUDIO_F32SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_F32; break;
	}

	return input_device_id;
}

SDL_AudioDeviceID Ultrasonic::init_device_output(GGWave::SampleFormat& o_sample_format, int& o_sample_rate, SDL_AudioSpec& o_output_device_spec)
{
	SDL_AudioSpec playbackSpec;
	SDL_zero(playbackSpec);

	playbackSpec.freq = GGWave::kBaseSampleRate;
	playbackSpec.format = AUDIO_S16SYS;
	playbackSpec.channels = 1;
	playbackSpec.samples = 16 * 1024;
	playbackSpec.callback = NULL;

	SDL_zero(o_output_device_spec);

	constexpr char* DEFAULT_DEVICE = nullptr;
	constexpr int OUTPUT_AUDIO = SDL_FALSE;
	constexpr int NO_CHANGES = 0;
	SDL_AudioDeviceID output_device_id = SDL_OpenAudioDevice(DEFAULT_DEVICE, OUTPUT_AUDIO, &playbackSpec, &o_output_device_spec, NO_CHANGES);

	if (!output_device_id) {
		const std::string log_message = std::string("Couldn't open an audio device for playback: ") + SDL_GetError();
		LOG(log_message);
		output_device_id = 0;
	}
	else if (o_output_device_spec.format != playbackSpec.format ||
			o_output_device_spec.channels != playbackSpec.channels ||
			o_output_device_spec.samples != playbackSpec.samples) {
			output_device_id = 0;
			SDL_CloseAudio();
			LOG("Failed to initialize playback SDL_OpenAudio!");
	}

	o_sample_rate = o_output_device_spec.freq;
	
	switch (o_output_device_spec.format) {
		case AUDIO_U8:      o_sample_format = GGWAVE_SAMPLE_FORMAT_U8;  break;
		case AUDIO_S8:      o_sample_format = GGWAVE_SAMPLE_FORMAT_I8;  break;
		case AUDIO_U16SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_U16; break;
		case AUDIO_S16SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_I16; break;
		case AUDIO_S32SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_F32; break;
		case AUDIO_F32SYS:  o_sample_format = GGWAVE_SAMPLE_FORMAT_F32; break;
	}

	return output_device_id;
}
	
}
