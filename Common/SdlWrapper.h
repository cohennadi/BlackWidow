#pragma once

namespace ultrasonic
{

class SdlWrapper
{
public:

	// Ctor
	SdlWrapper();
	
	// Dtor
	~SdlWrapper();

	// Deleted functions
	SdlWrapper(const SdlWrapper&) = delete;
	SdlWrapper(SdlWrapper&&) = delete;
	SdlWrapper& operator=(const SdlWrapper&) = delete;
	SdlWrapper& operator=(SdlWrapper&&) = delete;
private:

	// initialise sdl.
	static void init_sdl();
};
	
}
