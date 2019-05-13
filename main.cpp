#include "WavFile.h"

int main(int argc, char *argv[])
{
	WavFile<float> wav{};

	wav.load("samples/in.wav");
	wav.printSummary();

	return 0;
}
