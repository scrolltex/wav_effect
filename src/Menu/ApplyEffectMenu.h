#pragma once
#include "Menu.h"
#include "WavManager.h"

class ApplyEffectMenu : public Menu
{
public:
	explicit ApplyEffectMenu(std::stack<std::unique_ptr<Menu>>* menuStack);

	void onSelect() override;

private:
	WavManager& m_wm = WavManager::get();

	void monoToStereo() const;
	void reverse() const;
	void volume() const;
	void reverberation() const;
	void rotating() const;
};
