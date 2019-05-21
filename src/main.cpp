#include <iostream>
#include <filesystem>
#include "WavFile.h"
#include "Effects.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
namespace fs = std::filesystem;
using namespace effects;

/**
 * \brief Print usage to stdout
 * \param programName name of program binary
 */
void printUsage(std::string_view programName)
{
	cout << "Usage: " << programName << " in.wav [out.wav]" << endl;
}

// Print all available commands
void printAvailableCommands()
{
	// TODO: add available commands
	cout << "Available commands: " << endl;
}

int main(int argc, char *argv[])
{
	// If filepath not specified, or user type help - print usage
	if (argc < 2 || argc > 3 || strstr(argv[1], "help") != nullptr)
	{
		printUsage(fs::path(argv[0]).stem().u8string());
		return 0;
	}
	
	const fs::path filepath = argv[1]; // filepath to original wave file
	const fs::path out_filepath =  // filepath to output wave file
		argc == 3 ? argv[2] : fs::current_path() / ("out-" + filepath.stem().u8string() + ".wav");

	// check for file existence
	if (!fs::exists(filepath) || !fs::is_regular_file(filepath))
	{
		cerr << "File not exists, or it is not a file!" << endl;
		return 1;
	}

	// Wave file loader
	WavFile<float> wav;

	// Load wave file
	cout << "Loading wave file...";
	if (!wav.load(filepath.u8string()))
	{
		cerr << "Error: can`t load this file as wave file!" << endl;
		return 1;
	}
	cout << "Done" << endl;
	
	// Print wave file header
	cout << "File summary:" << endl;
	wav.printSummary();
	cout << endl;

	int cmd = 0;
	bool isRunning = true;	
	do // Commands loop
	{
		printAvailableCommands();
		cout << "Enter command: ";
		cin >> cmd;

		// TODO: add more commands
		// TODO: add effects params setup by console
		switch (cmd)
		{
			case -1: // Quit without saving
			{
				cout << "Unsaved data may be lost! You realy want to exit? (y/n): ";
				std::string answer;
				cin >> answer;
				if(answer == "y" || answer == "Y")
					isRunning = false;
				break;					
			}

			case 0: // Quit with saving
				// TODO: add lazy effects applying
				cout << "Applying effects..." << endl;

				if (wav.isMono())
					monoToStereo(wav);

				applyTremolo(wav, 10);
				applyRotatingStereo(wav, 1.f);
				applyReverberation(wav);
				applyFadeIn(wav, 2.f, Logarithmic);
				applyFadeOut(wav, 2.f, Linear);

				cout << "Saving... ";
				if (!wav.save(out_filepath.u8string()))
					cerr << "Saving failed!" << endl;
				else
				{
					cout << "Saved" << endl;
					isRunning = false;
				}
				break;

			default: cout << "Unknown command" << endl;
		}
	} while (isRunning);

	return 0;
}
