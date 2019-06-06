#pragma once
#include "WavFile.h"
#include "curve.h"

namespace effects
{
	/**
	 * \brief Convert mono sound to stereo
	 * \param wav wave file 
	 */
	void MonoToStereo(WavFile<float>& wav);

	/**
	 * \brief Apply rotation effect on stereo wave file
	 * \param wav wave file
	 * \param rate rotating rate, in seconds
	 * \throw invalid_argument file not in stereo, or rate <= 0
	 */
	void ApplyRotatingStereo(WavFile<float>& wav, float rate);

	/**
	 * \brief Increase volume by volume_db
	 * \param wav wave file
	 * \param volume_db How much dB increase
	 */
	void ApplyVolume(WavFile<float>& wav, float volume_db);
	
	/**
	 * \brief Apply effect of reversing the sound
	 * \param wav wave file
	 */
	void ApplyReverse(WavFile<float>& wav);

	/**
	 * \brief Apply delay effect
	 * \param wav wave file
	 * \param delay_millis Delay milliseconds
	 * \param decay Decay
	 * \throw out_of_range channel or delay time out of range
	 * \throw invalid_argument decay <= 0
	 */
	void ApplyDelay(WavFile<float>& wav, int delay_millis, float decay);

	/**
	 * \brief Apply delay effect
	 * \param wav wave file
	 * \param channel_idx Channel number for applying effect
	 * \param delay_millis Delay milliseconds
	 * \param decay Decay
	 * \throw out_of_range channel or delay time out of range
	 * \throw invalid_argument decay <= 0
	 */
	void ApplyDelay(WavFile<float>& wav, size_t channel_idx, int delay_millis, float decay);

	/**
	 * \brief Apply reverberation effect
	 * \param wav wave file
	 */
	void ApplyReverberation(WavFile<float>& wav);

	/**
	 * \brief Apple compressor effect
	 * \param wav wave file
	 * \param threshold Threshold, dB
	 * \param ratio Compressing ratio
	 */
	void ApplyCompressor(WavFile<float>& wav, float threshold, float ratio, bool downward = true);

	/**
	 * \brief Apply distortion effect
	 * \param wav wave file
	 * \param drive drive level (0..1)
	 * \param blend blending level of clean and distorted sound (0..1)
	 * \param volume volume level (0..1). Default is 1.
	 */
	void ApplyDistortion(WavFile<float>& wav, float drive, float blend, float volume = 1.f);
	
	/**
	 * \brief Apply fade in
	 * \param wav wave file
	 * \param time fade time in seconds
	 * \param curve_type fade curve type
	 * \throw invalid_argument time <= 0
	 */
	void ApplyFadeIn(WavFile<float>& wav, float time, CurveType curve_type = kLinear);

	/**
	 * \brief Apply fade out
	 * \param wav wave file
	 * \param time fade time in seconds
	 * \param curve_type fade curve type
	 * \throw invalid_argument time <= 0
	 */
	void ApplyFadeOut(WavFile<float>& wav, float time, CurveType curve_type = kLinear);

	/**
	 * \brief Apply tremolo effect
	 * \param wav wave file
	 * \param freq frequency of tremolo in Herz
	 * \param dry 
	 * \param wet
	 */
	void ApplyTremolo(WavFile<float>& wav, float freq, float dry = 0.5f, float wet = 0.5f);
}