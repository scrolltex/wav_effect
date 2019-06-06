#pragma once
#include "../Menu/MenuStateBase.h"
#include "../WavManager.h"

class MainMenu final : public MenuStateBase
{
public:
	explicit MainMenu(MenuStack* menu_stack);

	void OnSelect() override;

private:
	WavManager& wm_ = WavManager::get();

	void show_file_summary() const;
	void save() const;
	void quit() const;
};
