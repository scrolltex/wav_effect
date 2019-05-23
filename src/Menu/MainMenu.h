#pragma once
#include "MenuBase.h"
#include "WavManager.h"

class MainMenu : public MenuBase
{
public:
	explicit MainMenu(std::stack<std::unique_ptr<MenuBase>>* menuStack);

	void onSelect() override;

private:
	WavManager& m_wm = WavManager::get();

	void showFileSummary() const;
	void save() const;
	void quit();
};
