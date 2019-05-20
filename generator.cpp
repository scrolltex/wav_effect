#include "generator.h"
#include "utility.h"

std::vector<float> generateSilence(float length, float sampleRate)
{
	length = floor(length * sampleRate);
	std::vector<float> samples;
	std::fill_n(samples.begin(), length, 0);
	return samples;
}

std::vector<float> generateWaveInput(float freq, float length, float sampleRate, float phase)
{
	length = floor(length * sampleRate);
	phase *= sampleRate;
	const float factor = freq * (pi * 2) / sampleRate;

	std::vector<float> samples;
	for (size_t i = 0; i < length; i++)
		samples.push_back((i + phase) * factor);
	return samples;
}

std::vector<float> generateSineWave(float freq, float length, float sampleRate, float phase)
{
	auto wave = generateWaveInput(freq, length, sampleRate, phase);
	for (auto& sample : wave)
		sample = sin(sample);
	return wave;
}
