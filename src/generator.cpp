#include "generator.h"
#include "utility.h"

std::vector<float> GenerateSilence(float length, float sample_rate)
{
	length = floor(length * sample_rate);
	std::vector<float> samples;
	std::fill_n(samples.begin(), length, 0);
	return samples;
}

std::vector<float> GenerateWaveInput(float freq, float length, float sample_rate, float phase)
{
	length = ceil(length * sample_rate);
	phase *= sample_rate;
	const float factor = freq * (kPi * 2) / sample_rate;

	std::vector<float> samples;
	for (size_t i = 0; i < length; i++)
		samples.push_back((i + phase) * factor);
	return samples;
}

std::vector<float> GenerateSineWave(float freq, float length, float sample_rate, float phase)
{
	auto wave = GenerateWaveInput(freq, length, sample_rate, phase);
	for (auto& sample : wave)
		sample = sin(sample);
	return wave;
}
