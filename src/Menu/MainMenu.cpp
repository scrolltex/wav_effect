#include "MainMenu.h"
#include "ApplyEffectMenu.h"
#include <iostream>

using namespace std;

MainMenu::MainMenu(stack<unique_ptr<Menu>>* menuStack): Menu(menuStack)
{
	m_menuItems = {
		"Apply effect",
		"Save",
		"Quit"
	};
}

void MainMenu::onSelect()
{
	switch (selectedIndex)
	{
		case 0: // Apply effect
			navigateTo<ApplyEffectMenu>();
			break;

		case 1: // Save
			save();
			break;

		case 2: // Quit
			quit();
			break;

		default: ;
	}
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
	pressAnyKey();
}

void MainMenu::quit()
{
	if(m_wm.isFileUnsaved)
	{
		cout << "Wave file has unsaved changes that can be lost." << endl
			<< "You really want to quit?" << endl;

		if (!dialog())
			return;
	}

	cout << "Bye" << endl;
	back();
}
