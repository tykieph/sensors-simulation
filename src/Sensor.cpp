#include "Sensor.h"

#include <random>
#include <chrono>

/********************************************************************************/
Sensor::Sensor(int ID, int minValue, int maxValue, int frequency, 
	std::string sensorType, std::string encoderType)
	:
	m_ID(ID), m_MinValue(minValue), m_MaxValue(maxValue),
	m_Frequency(frequency), m_Type(sensorType), m_EncoderType(encoderType)
{
}
/********************************************************************************/
void Sensor::start(const std::atomic_bool &running, const std::atomic_bool &exit, 
	std::mutex &mtx, std::condition_variable &cv, SafeQueue<json> &messages)
{
	using namespace std::chrono_literals;

	while (!exit.load())
	{
		if (running.load())
		{
			auto value = get_value();
			auto quality = check_quality(value);

			/* create and push message onto the queue */
			auto message = Message(m_ID, value, m_Type, quality).get_msg();
			messages.push(message);

			std::this_thread::sleep_for(1000ms / m_Frequency);
		}
		else
		{
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock);
		}
	}
}
/********************************************************************************/
Quality Sensor::check_quality(const int value) const
{
	if (value < m_MinValue || value > m_MaxValue)
		return Quality::Alarm;

	Quality quality = Quality::Normal;

	int dist = m_MaxValue - m_MinValue;
	double result = (value - m_MinValue) / (double)dist;
	
	/* Warning or Alarm */
	if (result <= 0.25 || result >= 0.75)
	{
		if (result <= 0.10 || result >= 0.90)
			quality = Quality::Alarm;
		else
			quality = Quality::Warning;
	}

	return quality;
}
/********************************************************************************/
int Sensor::get_value() const
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(m_MinValue, m_MaxValue);

	return dist(rng);
}
/********************************************************************************/