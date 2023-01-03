#pragma once

#include <string>
#include <vector>

#include "Message.h"
#include "SafeQueue.h"

class Sensor
{
public:
	Sensor(int ID, int minValue, int maxValue, int frequency, 
		std::string sensorType, std::string encoderType);

	void start(const std::atomic_bool &running, const std::atomic_bool &exit, 
		std::mutex &mtx, std::condition_variable &cv, SafeQueue<json> &messages);
private:
	Quality check_quality(const int value) const;
	int get_value() const;
public:
private:
	int m_ID;
	int m_MinValue;
	int m_MaxValue;
	int m_Frequency;
	std::string m_Type;
	std::string m_EncoderType;
};

