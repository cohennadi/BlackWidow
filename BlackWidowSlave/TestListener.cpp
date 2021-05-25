#include "Ultrasonic.h"
#undef main

#include <ggwave/ggwave.h>
#include <iostream>
#include <mutex>
#include <thread>

int main()
{
	ultrasonic::Ultrasonic ultrasonic;
	auto ggWave = ultrasonic.get_ggwave(); 

	std::mutex mutex;

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		{
			std::lock_guard<std::mutex> lock(mutex);
			ultrasonic.run_logic();

			GGWave::TxRxData rxData;
			auto rxDataLength = ggWave->takeRxData(rxData);
			if (rxDataLength > 0)
			{
				std::string data(rxData.begin(), rxData.end());
				data.resize(rxDataLength);
				std::cout << data << std::endl;
			}
		}
	}
}
