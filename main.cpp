#include <iostream>
#include "WavFile.h"
#include "Effects.h"

int main(int argc, char *argv[])
{
	WavFile<float> wav;

	std::cout << "Loading file..." << std::endl;
	if (wav.load("samples/edm.wav"))
	{
		std::cout << "File loaded successfully. Summary: " << std::endl;
		wav.printSummary();

		effects::applyCompressor(wav, -6.f, 3.f);
		/*
		std::cout << "Applying delay effect" << std::endl;
		if (wav.isStereo())
		{
			std::cout << "Stereo file, using delay millis offsets." << std::endl;
			effects::applyDelay(wav, 0, 556, 0.65f);
			effects::applyDelay(wav, 0, 443, 0.65f);
			effects::applyDelay(wav, 1, 278, 0.5f);
			effects::applyDelay(wav, 1, 221, 0.5f);
		}
		else
		{
			effects::applyDelay(wav, 500, 0.65f);
			effects::applyDelay(wav, 250, 0.5f);
		}*/

		std::cout << "Saving output file..." << std::endl;
		if (wav.save("samples/out.wav"))
			std::cout << "Done successfully." << std::endl;
		else
			std::cout << "Saving failed!" << std::endl;
	}
	else
		std::cout << "Loading failed!" << std::endl;

	return 0;
}
