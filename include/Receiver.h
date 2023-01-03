#pragma once

#include <iostream>

#include "Message.h"

class Receiver
{
public:
	Receiver(const int ID, const int sensorID, const bool active);

	bool process_msg(const json &msg) const;
private:
	bool accept_msg(const json &msg) const;
	void analyze_msg(const json &msg) const;
public:
private:
	int m_ID;
	int m_SensorID;
	bool m_Active;
};

