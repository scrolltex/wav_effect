#pragma once
#include <cmath>

constexpr float pi = 3.14159265358979323846f;

/**
 * \brief Convert linear volume to decibels
 * \param volume linear volume
 * \return decibels
 */
inline float lin2db(float volume) noexcept
{
	static const float log_2_db = 20.f / log(10.f);
	return log(abs(volume) + 0.0001f)* log_2_db;
}

/**
 * \brief Conver decibels to linear volume
 * \param dB decibels
 * \return linear volume
 */
inline float db2lin(float dB) noexcept
{
	static const float db_2_log = log(10.f) / 20.f;
	return exp(dB * db_2_log);
}

template<typename T>
constexpr T sign(T val) noexcept
{
	return val >= T(0) ? 1 : -1;
}
