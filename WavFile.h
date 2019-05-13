#pragma once
#include <vector>
#include <string>

template<typename T>
class WavFile
{
public:
	typedef std::vector<std::vector<T>> AudioData;
	typedef std::vector<uint8_t> FileData;

	WavFile();

	bool save(std::string filename);

	bool load(std::string filename);
	
	int getNumChannels() const;
	void setNumChannels(int numChannels);

	int getNumSamplesPerChannel() const;
	void setNumSamplesPerChannel(int numSamples);

	double getLengthInSeconds() const;

	void printSummary() const;
	
	uint32_t sampleRate;
	int bitDepth;
	AudioData samples;

private:
	void clearSamples();

	int16_t twoBytesToInt(FileData& source, int startIndex);
	int32_t fourBytesToInt(FileData& source, int startIndex);
	int getIndexOfStr(FileData source, std::string str) const;

	T sixteenBitIntToSample(int16_t sample);
	int16_t sampleToSixteenBitInt(T sample);

	uint8_t sampleToSingleByte(T sample);
	T singleByteToSample(uint8_t sample);

	T clamp(T value, T minValue, T maxValue);
};