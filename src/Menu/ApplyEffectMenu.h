#pragma once
#include "MenuBase.h"
#include "WavManager.h"

class ApplyEffectMenu : public MenuBase
{
public:
	explicit ApplyEffectMenu(std::stack<std::unique_ptr<MenuBase>>* menuStack);

	void onSelect() override;

private:
	WavManager& m_wm = WavManager::get();

	void monoToStereo() const;
	void reverse() const;
	void volume() const;
	void reverberation() const;
	void rotating() const;
};
