#pragma once

#include <string>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <map>

#include "Sensor.h"
#include "Receiver.h"
#include "SafeQueue.h"
#include "Message.h"

/* message type */
using msg_t = json;

class Simulation
{
public:
	Simulation(const std::string &sensorsJSON, const std::string &receiversJSON);
	Simulation(const Simulation&) = delete;
	Simulation(Simulation&&) = delete;
	~Simulation();

	void run();
	void stop();
private:
	void transmit_messages();
	void get_sensors(const std::string &sensorsJSON);
	void get_receivers(const std::string &receiversJSON);
public:
private:
	std::vector<Sensor> m_Sensors;
	std::unordered_multimap<int, Receiver> m_Receivers;

	SafeQueue<msg_t> m_Messages;
	std::thread m_Transmitter;

	std::vector<std::thread> m_Threads;
	std::atomic_bool m_Running;
	std::atomic_bool m_Exit;
	std::mutex m_Mtx;
	std::condition_variable m_CV;
};

