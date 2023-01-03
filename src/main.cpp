#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <csignal>

#include "Simulation.h"

std::condition_variable cv;
std::mutex mtx;

void signal_handler(int signal);

int main(int argc, char **argv)
{
	using namespace std::chrono_literals;

	/* peacefully stop simulation when received SIGINT signal */
	std::signal(SIGINT, signal_handler);

	std::string sensorsJSON, receiversJSON;
	if (argc > 1)
	{
		sensorsJSON = argv[1];
		receiversJSON = argv[2];
	}
	else
	{
		sensorsJSON = "data/sensorConfig.json";
		receiversJSON = "data/receiverConfig.json";
	}

	Simulation sim(sensorsJSON, receiversJSON);

	sim.run();

	/* wait for sigint */
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait(lock);

	sim.stop();

	return 0;
}

void signal_handler(int signal)
{
	std::cout << "SIGINT received! Exitting..." << std::endl;
	cv.notify_one();
}