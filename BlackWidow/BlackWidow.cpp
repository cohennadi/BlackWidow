#include "Ultrasonic.h"
#undef main

#include <ggwave/ggwave.h>
#include <cstdio>
#include <string>
#include <mutex>
#include <thread>
#include <iostream>


int main(int argc, char** argv) {
	
    int captureId = 0;
    int playbackId = 0;
    int txProtocol =  GGWAVE_TX_PROTOCOL_ULTRASOUND_FASTEST;


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
    std::thread inputThread([&]() {
        std::string inputOld = "";
        while (true) {
            std::string input;
            std::cout << "Enter text: ";
            getline(std::cin, input);
            if (input.empty()) {
                std::cout << "Re-sending ... " << std::endl;
                input = inputOld;
            }
            else {
                std::cout << "Sending ... " << std::endl;
            }
            {
                std::lock_guard<std::mutex> lock(mutex);
                ggWave->init(input.size(), input.data(), ggWave->getTxProtocol(txProtocol), 70);
            }
            inputOld = input;
        }
        });


    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        {
            std::lock_guard<std::mutex> lock(mutex);
            ultrasonic.run_logic();
        }
    }

    inputThread.join();

    return 0;
}
