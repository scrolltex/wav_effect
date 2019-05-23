#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <functional>
#include <filesystem>
#include "Menu/MenuBase.h"
#include "Menu/MainMenu.h"
#include "Menu/WavManager.h"

using namespace std;
namespace fs = std::filesystem;

std::stack<unique_ptr<MenuBase>> menuStack;
int main(int argc, char** argv)
{
	// If filepath not specified, or user type help - print usage
	if (argc < 2 || argc > 3 || strstr(argv[1], "help") != nullptr)
	{
		const auto programName = fs::path(argv[0]).stem().u8string();
		cout << "Usage: " << programName << " in.wav [out.wav]" << endl;
		return 0;
	}

	auto& wm = WavManager::get();
	wm.filepath = argv[1]; // filepath to original wave file
	wm.out_filepath =  // filepath to output wave file
		argc == 3 ? argv[2] : fs::current_path() / ("out-" + wm.filepath.stem().u8string() + ".wav");

	// check for file existence
	if (!fs::exists(wm.filepath) || !fs::is_regular_file(wm.filepath))
	{
		cerr << "File not exists, or it is not a file!" << endl;
		return 1;
	}

	// Load file
	cout << "File is loading..." << endl;
	if(!wm.wav.load(wm.filepath.string()))
	{
		cerr << "File loading failed!" << endl;
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

		const auto title = menuStack.top()->getTitle();
		if (!title.empty())
		{
			cout << title << endl;
			cout << string(title.length(), '-') << endl;
		}

		for (size_t i = 0; i < currentMenuItems.size(); i++)
		{
			cout << (i == selectedIndex ? '>' : ' ');
			cout << currentMenuItems[i];
			cout << (i == selectedIndex ? '<' : ' ');
			cout << endl;
		}

		// Key handling
		auto keyCode = _getch();
		if (keyCode == Enter)  // Enter
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
				if (++selectedIndex >= currentMenuItems.size())
					selectedIndex = 0;
			}
		}
		else if (keyCode >= Key1 && keyCode <= Key9) // Select via numbers
		{
			const size_t idx = keyCode - Key1;
			if (idx < currentMenuItems.size())
			{
				menuStack.top()->selectedIndex = idx;
				menuStack.top()->onSelect();
			}
		}
	}

	return 0;
}
