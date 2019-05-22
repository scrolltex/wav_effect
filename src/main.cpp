#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <functional>
#include <filesystem>
#include "Menu/Menu.h"
#include "Menu/MainMenu.h"
#include "Menu/WavManager.h"

using namespace std;
namespace fs = std::filesystem;

std::stack<unique_ptr<Menu>> menuStack;

enum KeyCode
{
	Enter = 13,
	SpecialKeys = 224,
	ArrowUp = 72,
	ArrowDown = 80
};

int main(int argc, char** argv)
{
	// If filepath not specified, or user type help - print usage
	if (argc < 2 || argc > 3 || strstr(argv[1], "help") != nullptr)
	{
		const auto programName = fs::path(argv[0]).stem().u8string();
		cout << "Usage: " << programName << " in.wav [out.wav]" << endl;
		return 0;
	}
	
	WavManager& wm = WavManager::get();
	wm.filepath = argv[1]; // filepath to original wave file
	wm.out_filepath =  // filepath to output wave file
		argc == 3 ? argv[2] : fs::current_path() / ("out-" + wm.filepath.stem().u8string() + ".wav");

	// check for file existence
	if (!fs::exists(wm.filepath) || !fs::is_regular_file(wm.filepath))
	{
		cerr << "File not exists, or it is not a file!" << endl;
		return 1;
	}

	// Push MainMenu into stack
	menuStack.push(make_unique<MainMenu>(&menuStack));
	while(true)
	{
		// Quit if no menu to show
		if (menuStack.empty())
			break;

		const vector<string>& currentMenuItems = menuStack.top()->getMenuItems();
		size_t& selectedIndex = menuStack.top()->selectedIndex;

		//TODO: Print information about loaded file

		// Draw menu
		system("cls");
		for (size_t i = 0; i < currentMenuItems.size(); i++)
		{
			cout << (i == selectedIndex ? '>' : ' ');
			cout << currentMenuItems[i];
			cout << (i == selectedIndex ? '<' : ' ');
			cout << endl;
		}

		// Key handling
		auto keyCode = _getch();
		if(keyCode == Enter)  // Enter
		{
			menuStack.top()->onSelect();
		}
		else if (keyCode == SpecialKeys) // Navigation via arrows
		{
			keyCode = _getch();
			if (keyCode == ArrowUp) // Up
			{
				if (--selectedIndex == std::numeric_limits<size_t>::max())
					selectedIndex = currentMenuItems.size() - 1;
			}
			else if (keyCode == ArrowDown) // Down
			{
				if (++selectedIndex > currentMenuItems.size())
					selectedIndex = 0;
			}
		}
	}

	return 0;
}
