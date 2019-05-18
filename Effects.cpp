#include <algorithm>
#include <stdexcept>
#include "Effects.h"
#include "utility.h"

using std::clamp;

void effects::monoToStereo(WavFile<float>& wav)
{
	if (!wav.isMono())
		throw std::invalid_argument("Wave file must be mono");

	wav.setNumChannels(2);
	std::copy(wav.samples[0].begin(), wav.samples[0].end(), wav.samples[1].begin());
}

void effects::applyRotatingStereo(WavFile<float>& wav, float rate)
{
	if (!wav.isStereo())
		throw std::invalid_argument("Wave file must be a stereo");

	if (rate <= 0)
		throw std::invalid_argument("Rate must be greater than 0");

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

void effects::applyDelay(WavFile<float>& wav, size_t channelIdx, int delayMillis, float decay)
{
	if (channelIdx >= wav.getNumChannels())
		throw std::out_of_range("Channel");

	if (delayMillis <= 0 || delayMillis * 0.001 > wav.getLengthInSeconds())
		throw std::out_of_range("Delay time");

	if (decay <= 0)
		throw std::invalid_argument("Decay must be greater than 0");

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

void effects::applyFadeIn(WavFile<float>& wav, float time, CurveType curveType)
{
	if (time <= 0 || time > wav.getLengthInSeconds())
		throw std::invalid_argument("Invalid fade time");

	const float fade_samples = time * wav.sampleRate;
	for(auto& channel : wav.samples)
		for(size_t i = 0; i < fade_samples && i < wav.getNumSamplesPerChannel(); i++)
			channel[i] *= applyCurve(static_cast<float>(i) / fade_samples, curveType);
}

void effects::applyFadeOut(WavFile<float>& wav, float time, CurveType curveType)
{
	if (time <= 0 || time > wav.getLengthInSeconds())
		throw std::invalid_argument("Invalid fade time");

	const size_t samples_count = wav.getNumSamplesPerChannel();
	const float fade_samples = time * wav.sampleRate; // fade time in samples
	const size_t start_pos = samples_count - fade_samples; // sample that starts

	for (auto& channel : wav.samples)
		for (size_t i = start_pos; i < samples_count; i++)
			channel[i] *= 1.f - applyCurve(static_cast<float>(i - start_pos) / fade_samples, curveType);
}
