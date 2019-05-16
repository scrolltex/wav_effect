#pragma once
#include <cmath>

constexpr float pi = 3.14159265358979323846f;

inline float lin2db(float volume)
{
	static const float log_2_db = 20.f / log(10.f);
	return log(volume)* log_2_db;
}

inline float db2lin(float dB)
{
	static const float db_2_log = log(10.f) / 20.f;
	return exp(dB * db_2_log);
}

template<typename T>
T sign(T val) noexcept
{
	return val >= T(0) ? 1 : -1;
}
