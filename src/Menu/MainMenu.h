#pragma once
#include "Menu.h"
#include "WavManager.h"

class MainMenu : public Menu
{
public:
	explicit MainMenu(std::stack<std::unique_ptr<Menu>>* menuStack);

	void onSelect() override;

private:
	WavManager& m_wm = WavManager::get();

	void save() const;
	void quit();
};
