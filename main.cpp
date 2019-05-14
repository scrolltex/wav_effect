#include "WavFile.h"

void applyDelay(WavFile<float>& wav, int delayMillis, float decay)
{
	const int delaySamples = static_cast<int>(static_cast<float>(delayMillis) * (wav.sampleRate / 1000.f));
	for (auto& channel : wav.samples)
		for (size_t i = 0; i < channel.size() - delaySamples; i++)
			channel[i + delaySamples] += channel[i] * decay;
}

int main(int argc, char *argv[])
{
	WavFile<float> wav;

	if(wav.load("samples/idea.wav"))
	{
		wav.printSummary();
		applyDelay(wav, 250, 0.5f);
		wav.save("samples/out_delay.wav");		
	}

	return 0;
}
