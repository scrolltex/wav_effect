#include <algorithm>
#include "Effects.h"

using std::clamp;

constexpr float pi = 3.14159265358979323846;

void effects::applyVolume(WavFile<float>& wav, float volumeFactor)
{
	for (auto& channel : wav.samples)
		for (auto& sample : channel)
			sample *= volumeFactor;
}

void effects::applyReverse(WavFile<float>& wav)
{
	for (auto& channel : wav.samples)
		std::reverse(channel.begin(), channel.end());
}

void effects::applyDelay(WavFile<float>& wav, int delayMillis, float decay)
{
	for (int i = 0; i < static_cast<int>(wav.samples.size()); i++)
		applyDelay(wav, i, delayMillis, decay);
}

void effects::applyDelay(WavFile<float>& wav, int channelIdx, int delayMillis, float decay)
{
	const int delaySamples = static_cast<int>(static_cast<float>(delayMillis) * (wav.sampleRate / 1000.f));
	for (size_t i = 0; i < wav.samples[channelIdx].size() - delaySamples; i++)
		wav.samples[channelIdx][i + delaySamples] += wav.samples[channelIdx][i] * decay;
}

void effects::applyDistortion(WavFile<float>& wav, float drive, float blend, float volume)
{
	const float range = 1000.f;

	drive = clamp<float>(drive, 0.f, 1.f);
	blend = clamp<float>(blend, 0.f, 1.f);
	volume = clamp<float>(volume, 0.f, 1.f);

	for (auto& channel : wav.samples)
	{
		for (auto& sample : channel)
		{
			const auto cleanSample = sample;

			sample *= drive * range;
			sample = (2.f / pi * static_cast<float>(atan(sample)) * blend + cleanSample * (1.f - blend)) / 2.f * volume;
		}
	}
}
