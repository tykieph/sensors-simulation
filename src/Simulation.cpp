#include "Simulation.h"

#include <fstream>
#include <thread>
#include <functional>

namespace fs = std::filesystem;

/********************************************************************************/
Simulation::Simulation(const std::string& sensorsJSON, const std::string &receiversJSON)
	: m_Running(false), m_Exit(false)
{
	get_sensors(sensorsJSON);
	get_receivers(receiversJSON);

	/* start transmitting available messages */
	m_Transmitter = std::thread(&Simulation::transmit_messages, this);
	m_Transmitter.detach();

	/* start every sensor on a different thread */
	for (auto& s: m_Sensors)
	{
		m_Threads.emplace_back(
			&Sensor::start, &s, 
			std::ref(m_Running), std::ref(m_Exit), 
			std::ref(m_Mtx), std::ref(m_CV), std::ref(m_Messages));
	}
}
/********************************************************************************/
Simulation::~Simulation()
{
	/* set exit flag to true */
	m_Exit.store(true);

	/* and notify all waiting threads */
	m_CV.notify_all();

	/* wait for threads to join */
	for (auto &t : m_Threads)
	{
		if (t.joinable())
			t.join();
	}
}
/********************************************************************************/
void Simulation::run()
{
	/* tell threads to start working */
	m_Running.store(true);

	/* and notify all waiting threads to start working */
	m_CV.notify_all();
}
/********************************************************************************/
void Simulation::stop()
{
	/* tell threads to stop working */
	m_Running.store(false);
}
/********************************************************************************/
void Simulation::get_sensors(const std::string& sensorsJSON)
{
	/* get sensors data from a config JSON file */
	std::ifstream sensorConfig(sensorsJSON);

	if (!sensorConfig.is_open())
	{
		std::cerr << "ERROR! Couldn't open sensors JSON file! (" << fs::absolute(sensorsJSON) << ")\n";
		exit(1);
	}

	/* parse json */
	json sensors = json::parse(sensorConfig);

	/* create sensor objects */
	for (auto &s : sensors["Sensors"].items())
	{
		auto &elem = s.value();
		m_Sensors.emplace_back(elem["ID"], elem["MinValue"], elem["MaxValue"], 
			elem["Frequency"], elem["Type"], elem["EncoderType"]);
	}
}
/********************************************************************************/
void Simulation::get_receivers(const std::string &receiversJSON)
{
	/* get sensors data from a config JSON file */
	std::ifstream receiverConfig(receiversJSON);

	if (!receiverConfig.is_open())
	{
		std::cerr << "ERROR! Couldn't open receivers JSON file! (" << fs::absolute(receiversJSON) << ")\n";
		exit(1);
	}

	/* parse json */
	json receivers = json::parse(receiverConfig);

	/* create receiver objects */
	for (auto &r : receivers["Receivers"].items())
	{
		auto &elem = r.value();
		m_Receivers.emplace(elem["SensorID"], 
			Receiver(elem["ID"], elem["SensorID"], elem["Active"]));
	}
}
/********************************************************************************/
void Simulation::transmit_messages()
{
	while (true)
	{
		msg_t msg;

		/* wait for message to appear in queue */
		m_Messages.wait_pop(msg);

		/* get sensor id from message */
		auto sensorID = Message(msg).get_sensor_id();

		/* send message to corresponding receivers */
		auto range = m_Receivers.equal_range(sensorID);
		for (auto& it = range.first; it != range.second; ++it)
		{
			auto& [id, receiver] = *it;
			receiver.process_msg(msg);
		}
	}
}
/********************************************************************************/