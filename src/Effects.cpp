#include <algorithm>
#include <stdexcept>
#include <vector>
#include "Effects.h"
#include "utility.h"
#include "generator.h"

using std::clamp;

void effects::MonoToStereo(WavFile<float>& wav)
{
	if (!wav.IsMono())
		throw std::invalid_argument("Wave file must be mono");

	wav.SetNumChannels(2);
	std::copy(wav.samples[0].begin(), wav.samples[0].end(), wav.samples[1].begin());
}

void effects::ApplyRotatingStereo(WavFile<float>& wav, float rate)
{
	if (!wav.IsStereo())
		throw std::invalid_argument("Wave file must be a stereo");

	if (rate <= 0)
		throw std::invalid_argument("Rate must be greater than 0");

	for (size_t i = 0; i < wav.GetNumSamplesPerChannel(); i++)
	{
		const float x = static_cast<float>(i) / static_cast<float>(wav.sampleRate) * rate;
		wav.samples[0][i] *= sin(x);
		wav.samples[1][i] *= cos(x);
	}
}

void effects::ApplyVolume(WavFile<float>& wav, float volume_db)
{
	for (auto& channel : wav.samples)
		for (auto& sample : channel)
			sample = db_to_lin(lin_to_db(sample) + volume_db);
}

void effects::ApplyReverse(WavFile<float>& wav)
{
	for (auto& channel : wav.samples)
		std::reverse(channel.begin(), channel.end());
}

void effects::ApplyDelay(WavFile<float>& wav, int delay_millis, float decay)
{
	for (int i = 0; i < static_cast<int>(wav.samples.size()); i++)
		ApplyDelay(wav, i, delay_millis, decay);
}

void effects::ApplyDelay(WavFile<float>& wav, size_t channel_idx, int delay_millis, float decay)
{
	if (channel_idx >= wav.GetNumChannels())
		throw std::out_of_range("Channel");

	if (delay_millis <= 0 || delay_millis * 0.001 > wav.GetLengthInSeconds())
		throw std::out_of_range("Delay time");

	if (decay <= 0)
		throw std::invalid_argument("Decay must be greater than 0");

	const int delaySamples = static_cast<int>(static_cast<float>(delay_millis) * (wav.sampleRate / 1000.f));
	for (size_t i = 0; i < wav.samples[channel_idx].size() - delaySamples; i++)
		wav.samples[channel_idx][i + delaySamples] += wav.samples[channel_idx][i] * decay;
}

void effects::ApplyReverberation(WavFile<float>& wav)
{
	// TODO: Make reverberation customizable
	ApplyDelay(wav, 100, 0.75f);
	ApplyDelay(wav, 250, 0.35f);
	ApplyDelay(wav, 500, 0.15f);
}

void effects::ApplyCompressor(WavFile<float>& wav, float threshold, float ratio, bool downward)
{
	for (auto& channel : wav.samples)
	{
		for (auto& sample : channel)
		{
			const auto sample_db = lin_to_db(sample);

			if (downward)
			{
				if (sample_db > threshold)
					sample = sign(sample) * db_to_lin((sample_db - threshold) / ratio + threshold);				
			}
			else
			{
				if (sample_db < threshold)
					sample = sign(sample) * db_to_lin(threshold - ((threshold - sample_db) / ratio));				
			}
		}
	}
}

void effects::ApplyDistortion(WavFile<float>& wav, float drive, float blend, float volume)
{
	const float range = 1000.f;

	drive = clamp<float>(drive, 0.f, 1.f);
	blend = clamp<float>(blend, 0.f, 1.f);
	volume = clamp<float>(volume, 0.f, 1.f);

	for (auto& channel : wav.samples)
	{
		for (auto& sample : channel)
		{
			const auto clean_sample = sample;

			sample *= drive * range;
			sample = (2.f / kPi * static_cast<float>(atan(sample)) * blend + clean_sample * (1.f - blend)) / 2.f * volume;
		}
	}
}

void effects::ApplyFadeIn(WavFile<float>& wav, float time, CurveType curve_type)
{
	if (time <= 0 || time > wav.GetLengthInSeconds())
		throw std::invalid_argument("Invalid fade time");

	const float fade_samples = time * wav.sampleRate;
	for(auto& channel : wav.samples)
		for(size_t i = 0; i < fade_samples && i < wav.GetNumSamplesPerChannel(); i++)
			channel[i] *= ApplyCurve(static_cast<float>(i) / fade_samples, curve_type);
}

void effects::ApplyFadeOut(WavFile<float>& wav, float time, CurveType curve_type)
{
	if (time <= 0 || time > wav.GetLengthInSeconds())
		throw std::invalid_argument("Invalid fade time");

	const size_t samples_count = wav.GetNumSamplesPerChannel();
	const size_t fade_samples = time * wav.sampleRate; // fade time in samples
	const size_t start_pos = samples_count - fade_samples; // sample that starts

	for (auto& channel : wav.samples)
		for (size_t i = start_pos; i < samples_count; i++)
			channel[i] *= 1.f - ApplyCurve(static_cast<float>(i - start_pos) / fade_samples, curve_type);
}

void effects::ApplyTremolo(WavFile<float>& wav, float freq, float dry, float wet)
{
	dry = clamp(dry, 0.f, 1.f);
	wet = clamp(wet, 0.f, 1.f);

	const auto sine_wave = GenerateSineWave(freq, 
		static_cast<float>(wav.GetNumSamplesPerChannel()) / wav.sampleRate, 
		wav.sampleRate);
	
	for (auto& channel : wav.samples)
		for (size_t i = 0; i < channel.size(); i++)
			channel[i] = (channel[i] * dry) + ((channel[i] * (sine_wave[i] / 2.f + 0.5f)) * wet);
}
