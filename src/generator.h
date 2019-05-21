#pragma once
#include <vector>

/**
 * \brief Generrate silence of specified length
 * \param length length of silence
 * \param sampleRate sample rate of wave
 */
std::vector<float> generateSilence(float length, float sampleRate = 44100);

/**
 * \brief Generate input for other wave generators
 * \param freq frequency
 * \param length of wave
 * \param sampleRate sample rate of wave
 * \param phase phase shift
 */
std::vector<float> generateWaveInput(float freq, float length, float sampleRate = 44100, float phase = 0);

/**
 * \brief Generate sine wave
 * \param freq frequency
 * \param length length of wave
 * \param sampleRate sample rate of wave
 * \param phase phase shift 
 */
std::vector<float> generateSineWave(float freq, float length, float sampleRate = 44100, float phase = 0);
