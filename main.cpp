#include <iostream>
#include "WavFile.h"
#include "Effects.h"

int main(int argc, char *argv[])
{
	WavFile<float> wav;

	std::cout << "Loading file..." << std::endl;
	if (wav.load("samples/guitar_32bit_stereo.wav"))
	{
		std::cout << "File loaded successfully. Summary: " << std::endl;
		wav.printSummary();

		if(wav.isMono())
			effects::monoToStereo(wav);

		effects::applyRotatingStereo(wav, 1.f);
		effects::applyReverberation(wav);
		effects::applyFadeIn(wav, 2.f);
		effects::applyFadeOut(wav, 2.f);

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
