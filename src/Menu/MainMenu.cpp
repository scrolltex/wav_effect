#include <iostream>
#include "MainMenu.h"
#include "ApplyEffectMenu.h"

using namespace std;

MainMenu::MainMenu(MenuStack* menuStack) : MenuBase(menuStack)
{
	m_title = "Main menu";
	m_menuItems = {
		"Show file summary",
		"Apply effect",
		"Save",
		"Quit"
	};
}

void MainMenu::onSelect()
{
	switch (selectedIndex)
	{
		case 0: // Show file summary
			showFileSummary();
			break;

		case 1: // Apply effect
			navigateTo<ApplyEffectMenu>();
			break;

		case 2: // Save
			save();
			break;

		case 3: // Quit
			quit();
			break;

		default: 
			throw std::exception("Unknown menu item index: " + selectedIndex);
	}
}

void MainMenu::showFileSummary() const
{
	system("cls");
	cout << " Loaded file: " << m_wm.filepath.string() << endl;
	m_wm.wav.printSummary();
	waitForEscape();
}

void MainMenu::save() const
{
	system("cls");

	cout << "Saving to: " << m_wm.out_filepath << endl;
	if (m_wm.wav.save(m_wm.out_filepath.string()))
		cout << "Done" << endl;
	else
		cout << "Failed" << endl;

	m_wm.isFileUnsaved = false;
	waitForEscape();
}

void MainMenu::quit() const
{
	if(m_wm.isFileUnsaved)
	{
		const auto msg = "Wave file has unsaved changes that can be lost.\nYou really want to quit?";
		if (!dialog(msg))
			return;
	}

	cout << "Bye" << endl;
	back();
}
