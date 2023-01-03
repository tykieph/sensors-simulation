#include "Receiver.h"

/* logging library */
#include "spdlog/spdlog.h"

#define MESSAGE_NORMAL(...)		spdlog::info(__VA_ARGS__)
#define MESSAGE_WARNING(...)	spdlog::warn(__VA_ARGS__)
#define MESSAGE_ALARM(...)		spdlog::error(__VA_ARGS__)

/********************************************************************************/
Receiver::Receiver(const int ID, const int sensorID, const bool active)
	: m_ID(ID), m_SensorID(sensorID), m_Active(active)
{
	spdlog::set_pattern("[%H:%M:%S:%e] [%^%-7l%$] %v");
}
/********************************************************************************/
bool Receiver::process_msg(const json &msg) const
{
	if (!accept_msg(msg))
		return false;

	analyze_msg(msg);

	return true;
}
/********************************************************************************/
bool Receiver::accept_msg(const json &msg) const
{
	return m_Active;
}
/********************************************************************************/
void Receiver::analyze_msg(const json &msg) const
{
	/* create message based on given JSON */
	Message m(msg);

	/* get data from message */
	auto quality = m.get_quality();
	auto str = m.to_string();
;
	/* log message */
	switch (quality)
	{
	case Quality::Normal:
		MESSAGE_NORMAL(str);
		break;
	case Quality::Warning:
		MESSAGE_WARNING(str);
		break;
	case Quality::Alarm:
		MESSAGE_ALARM(str);
		break;
	}
}
/********************************************************************************/