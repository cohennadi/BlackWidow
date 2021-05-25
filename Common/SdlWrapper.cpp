
#include "SdlWrapper.h"
#include "Defines.h"

#include <cassert>
#include <SDL.h>


using namespace ultrasonic;

SdlWrapper::SdlWrapper()
{
	init_sdl();
}

SdlWrapper::~SdlWrapper()
{
	try {
		SDL_CloseAudio();
		SDL_Quit();
	} catch(...) {
		LOG("error in sdl detor");
		assert(true);
	}
}

void SdlWrapper::init_sdl()
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


