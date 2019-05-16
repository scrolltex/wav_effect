#pragma once
#include "WavFile.h"

namespace effects
{
	/**
	 * \brief Increase volume by volume_db
	 * \param wav wave file
	 * \param volume_db How much dB increase
	 */
	void applyVolume(WavFile<float>& wav, float volume_db);
	
	/**
	 * \brief Apply effect of reversing the sound
	 * \param wav wave file
	 */
	void applyReverse(WavFile<float>& wav);

	/**
	 * \brief Apply delay effect to wave file
	 * \param wav wave file
	 * \param delayMillis Delay milliseconds
	 * \param decay Decay
	 */
	void applyDelay(WavFile<float>& wav, int delayMillis, float decay);

	/**
	 * \brief Apply delay effect to wave file
	 * \param wav wave file
	 * \param channelIdx Channel number for applying effect
	 * \param delayMillis Delay milliseconds
	 * \param decay Decay
	 */
	void applyDelay(WavFile<float>& wav, int channelIdx, int delayMillis, float decay);
	
	/**
	 * \brief Apple compressor effect to wave file
	 * \param wav wave file
	 * \param threshold Threshold, dB
	 * \param ratio Compressing ratio
	 */
	void applyCompressor(WavFile<float>& wav, float threshold, float ratio);

	/**
	 * \brief Apply distortion effect to wave file
	 * \param wav wave file
	 * \param drive drive level (0..1)
	 * \param blend blending level of clean and distorted sound (0..1)
	 * \param volume volume level (0..1). Default is 1.
	 */
	void applyDistortion(WavFile<float>& wav, float drive, float blend, float volume = 1.f);
}
