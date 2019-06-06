#include <iostream>
#include "MainMenu.h"
#include "ApplyEffectMenu.h"

using namespace std;

MainMenu::MainMenu(MenuStack* menu_stack) : MenuStateBase(menu_stack)
{
	title_ = "Main menu";
	menu_items_ = {
		"Show file summary",
		"Apply effect",
		"Save",
		"Quit"
	};
}

void MainMenu::OnSelect()
{
	switch (selected_index_)
	{
		case 0: // Show file summary
			show_file_summary();
			break;

		case 1: // Apply effect
			NavigateTo<ApplyEffectMenu>();
			break;

		case 2: // Save
			save();
			break;

		case 3: // Quit
			quit();
			break;

		default: 
			throw out_of_range("Unknown menu item index" + to_string(selected_index_));
	}
}

void MainMenu::show_file_summary() const
{
	system("cls");
	cout << " Loaded file: " << wm_.filepath.string() << endl;
	wm_.wav.PrintSummary();
	WaitForEscape();
}

void MainMenu::save() const
{
	system("cls");

	cout << "Saving to: " << wm_.out_filepath << endl;
	if (wm_.wav.Save(wm_.out_filepath.string()))
		cout << "Done" << endl;
	else
		cout << "Failed" << endl;

	wm_.isFileUnsaved = false;
	WaitForEscape();
}

void MainMenu::quit() const
{
	if(wm_.isFileUnsaved)
	{
		const auto msg = "Wave file has unsaved changes that can be lost.\nYou really want to quit?";
		if (!Ask(msg))
			return;
	}

	cout << "Bye" << endl;
	Back();
}
