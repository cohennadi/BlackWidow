#include "Agent.h"

#include <chrono>

using namespace std::literals;

int main(int argc, char** argv)
{
	Agent agent;
	agent.execute("C:\\encryptMe");
	std::this_thread::sleep_for(100s);
	
	return 0;
}
