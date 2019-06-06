#pragma once
#include <vector>

/**
 * \brief Generrate silence of specified length
 * \param length length of silence
 * \param sample_rate sample rate of wave
 */
std::vector<float> GenerateSilence(float length, float sample_rate = 44100);

/**
 * \brief Generate input for other wave generators
 * \param freq frequency
 * \param length of wave
 * \param sample_rate sample rate of wave
 * \param phase phase shift
 */
std::vector<float> GenerateWaveInput(float freq, float length, float sample_rate = 44100, float phase = 0);

/**
 * \brief Generate sine wave
 * \param freq frequency
 * \param length length of wave
 * \param sample_rate sample rate of wave
 * \param phase phase shift 
 */
std::vector<float> GenerateSineWave(float freq, float length, float sample_rate = 44100, float phase = 0);
