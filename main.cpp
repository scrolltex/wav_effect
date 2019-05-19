#include <iostream>
#include "WavFile.h"
#include "Effects.h"

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
	WavFile<float> wav;

	cout << "Loading file..." << endl;
	if (wav.load("samples/edm.wav"))
	{
		cout << "File loaded successfully. Summary: " << endl;
		wav.printSummary();

		if (wav.isMono())
		{
			cout << "File in mono. Converting to stereo..." << endl;
			effects::monoToStereo(wav);
		}

		cout << "Applying effects..." << endl;
		effects::applyRotatingStereo(wav, 1.f);
		effects::applyReverberation(wav);
		effects::applyFadeIn(wav, 2.f, Logarithmic);
		effects::applyFadeOut(wav, 2.f, Linear);

		cout << "Saving output file..." << endl;
		if (wav.save("samples/out.wav"))
			cout << "Done successfully." << endl;
		else
			cerr << "Saving failed!" << endl;
	}
	else
		cerr << "Loading failed!" << endl;

	return 0;
}
