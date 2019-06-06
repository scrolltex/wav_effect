#pragma once
#include "../Menu/MenuStateBase.h"
#include "../WavManager.h"

class ApplyEffectMenu final : public MenuStateBase
{
public:
	explicit ApplyEffectMenu(MenuStack* menu_stack);

	void OnSelect() override;

private:
	WavManager& wm_ = WavManager::get();

	void mono_to_stereo() const;
	void reverse() const;
	void volume() const;
	void reverberation() const;
	void rotating() const;
	void fade() const;
	void tremolo() const;
	void delay() const;
	void compressor() const;
	void distortion() const;

	static bool GreaterThanZero(float value)
	{
		return value > 0;
	}

	static bool IsNormalizedValue(float value)
	{
		return value >= 0.f && value <= 1.f;
	}
};
