#pragma once

#include <string>

// nlohmann json: https://github.com/nlohmann/json
#include "nlohmann/json.hpp"
using json = nlohmann::json;


enum class Quality
{
	Normal = 0,
	Warning,
	Alarm
};

NLOHMANN_JSON_SERIALIZE_ENUM(Quality, {
	{ Quality::Normal, "Normal" },
	{ Quality::Warning, "Warning" },
	{ Quality::Alarm, "Alarm" },
})


class Message
{
public:
	Message(const int id, const int value, const std::string type, Quality quality)
		: m_SensorID(id), m_Value(value), m_Type(type), m_Quality(quality) {}

	Message(const json &msg)
	{
		m_SensorID = msg["SensorID"];
		m_Value = msg["Value"];
		m_Type = msg["Type"];
		m_Quality = quality_from_string(msg["Quality"]);
	}

	json get_msg() const
	{
		json j = {
			{ "BEGIN", "$FIX" },
			{ "SensorID", m_SensorID },
			{ "Value", m_Value },
			{ "Type", m_Type },
			{ "Quality", quality_to_string(m_Quality) },
			{ "END", "*" },
		};

		return j;
	}

	int get_sensor_id() const { return m_SensorID; }
	int get_value() const { return m_Value; }
	std::string get_type() const { return m_Type; }
	Quality get_quality() const { return m_Quality; }

	std::string to_string() const
	{
		std::string msg =
			"Sensor {"
			" id: " + std::to_string(m_SensorID) +
			"; type: " + m_Type +
			" } -> " +
			"Message {"
			" value: " + std::to_string(m_Value) +
			"; quality: " + quality_to_string(m_Quality) +
			" }";

		return msg;
	}
private:
	std::string quality_to_string(const Quality q) const
	{
		json j = q;
		return j;
	}

	Quality quality_from_string(const std::string str) const
	{
		json j = str;
		return j.get<Quality>();
	}
public:
private:
	int m_SensorID;
	int m_Value;
	std::string m_Type;
	Quality m_Quality;
};
/********************************************************************************/