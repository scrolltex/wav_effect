#include <algorithm>
#include "Effects.h"
#include "utility.h"

using std::clamp;

void effects::monoToStereo(WavFile<float>& wav)
{
	if (!wav.isMono())
		return;

	wav.setNumChannels(2);
	std::copy(wav.samples[0].begin(), wav.samples[0].end(), wav.samples[1].begin());
}

void effects::applyRotatingStereo(WavFile<float>& wav, float rate)
{
	if (!wav.isStereo() || rate == 0)
		return;

	for (size_t i = 0; i < wav.getNumSamplesPerChannel(); i++)
	{
		const float x = static_cast<float>(i) / static_cast<float>(wav.sampleRate) * rate;
		wav.samples[0][i] *= sin(x);
		wav.samples[1][i] *= cos(x);
	}
}

void effects::applyVolume(WavFile<float>& wav, float volume_db)
{
	for (auto& channel : wav.samples)
		for (auto& sample : channel)
			sample = db2lin(lin2db(sample) + volume_db);
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

void effects::applyReverberation(WavFile<float>& wav)
{
	// TODO: Make reverberation customizable
	applyDelay(wav, 100, 0.75f);
	applyDelay(wav, 250, 0.35f);
	applyDelay(wav, 500, 0.15f);
}

void effects::applyCompressor(WavFile<float>& wav, float threshold, float ratio)
{
	for (auto& channel : wav.samples)
	{
		for (auto& sample : channel)
		{
			const double key_dB = lin2db(abs(sample) + 0.000001f);
			double over_dB = key_dB - threshold;
			if (over_dB < 0.0)
				over_dB = 0.0;

			const auto gr = over_dB * (1./ratio - 1.);
			sample *= db2lin(gr);
		}
	}
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
