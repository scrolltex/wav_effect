#pragma once
#include "WavFile.h"

namespace effects
{
	/**
	 * \brief Apply 
	 * \param wav 
	 * \param volumeFactor 
	 */
	void applyVolume(WavFile<float>& wav, float volumeFactor);
	
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
	 * \brief Apply distortion effect to wave file
	 * \param wav wave file
	 * \param drive drive level (0..1)
	 * \param blend blending level of clean and distorted sound (0..1)
	 * \param volume volume level (0..1). Default is 1.
	 */
	void applyDistortion(WavFile<float>& wav, float drive, float blend, float volume = 1.f);
}
