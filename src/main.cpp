#include <iostream>
#include <string>
#include <functional>
#include <filesystem>
#include "Menu/Menu.h"
#include "WavManager.h"
#include "MenuStates/MainMenu.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
	// If filepath not specified, or user type help - print usage
	if (argc < 2 || argc > 3 || strstr(argv[1], "help") != nullptr)
	{
		const auto program_name = fs::path(argv[0]).stem().u8string();
		cout << "Usage: " << program_name << " in.wav [out.wav]" << endl;
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
	if (!wm.wav.Load(wm.filepath.string()))
	{
		cerr << "File loading failed!" << endl;
		return 1;
	}

	// Run menu
	return RunMenu<MainMenu>();
}
