#include "MainMenu.h"
#include "ApplyEffectMenu.h"
#include <iostream>

using namespace std;

MainMenu::MainMenu(stack<unique_ptr<MenuBase>>* menuStack) : MenuBase(menuStack)
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

		default: ;
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