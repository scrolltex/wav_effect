#pragma once
#include "MenuBase.h"
#include "WavManager.h"

class MainMenu final : public MenuBase
{
public:
	explicit MainMenu(MenuStack* menuStack);

	void onSelect() override;

private:
	WavManager& m_wm = WavManager::get();

	void showFileSummary() const;
	void save() const;
	void quit() const;
};
