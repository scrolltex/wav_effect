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

/**
 * \brief Run menu
 * \tparam BaseState First menu state
 * \returns Exit code
 */
template<typename BaseState>
int runMenu()
{
	static_assert(std::is_base_of<MenuBase, BaseState>::value, "BaseState must inherit from MenuBase");

	// Push MainMenu into stack
	MenuStack stack;
	stack.push(make_unique<BaseState>(&stack));

	while (true)
	{
		// Quit if no menu to show
		if (stack.empty())
			break;

		const auto& currentMenuItems = stack.top()->getMenuItems();
		auto& selectedIndex = stack.top()->selectedIndex;

		// Draw menu
		system("cls");

		const auto title = stack.top()->getTitle();
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
			try
			{
				stack.top()->onSelect();
			}
			catch (std::exception& ex)
			{
				std::cerr << "Error: " << ex.what() << endl;
				return 1;
			}
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
				stack.top()->selectedIndex = idx;

				try
				{
					stack.top()->onSelect();
				}
				catch (std::exception& ex)
				{
					std::cerr << "Error: " << ex.what() << endl;
					return 1;
				}
			}
		}
	}

	return 0;
}

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

	// Run menu
	const auto exitCode = runMenu<MainMenu>();
	return exitCode;
}
