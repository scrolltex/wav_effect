#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "WavFile.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

template <typename T>
WavFile<T>::WavFile()
{
	bitDepth = 16;
	sampleRate = 44100;
	samples.resize(1);
	samples[0].resize(0);
}

template <typename T>
bool WavFile<T>::save(std::string filename)
{
	// UNDONE: saving
	return false;
}

template <typename T>
bool WavFile<T>::load(std::string filename)
{
	// Open file stream
	std::ifstream file(filename, std::ios::binary);

	if (!file.good())
	{
		cerr << "Error: Can`t open file: " << filename << endl;
		return false;
	}

	file.unsetf(std::ios::skipws);
	std::istream_iterator<uint8_t> begin(file), end;
	FileData fileData(begin, end);

	////////////////////////////////////
	// Read header chunk ///////////////
	const string headerChunkId(fileData.begin(), fileData.begin() + 4);
	const string format(fileData.begin() + 8, fileData.begin() + 12);

	// Find indexies of format and data chunks
	const auto formatChunkIndex = getIndexOfStr(fileData, "fmt"); 
	const auto dataChunkIndex = getIndexOfStr(fileData, "data");

	// Header must start with RIFF, format must be a WAVE, and file must contains format and data chunks
	if (headerChunkId != "RIFF" || format != "WAVE" || formatChunkIndex == -1 || dataChunkIndex == -1)
	{
		cerr << "Error: Invalid .wav file." << endl;
		return false;
	}

	////////////////////////////////////
	// Format chunk ////////////////////
	string formatChunkId(fileData.begin() + formatChunkIndex, fileData.begin() + formatChunkIndex + 4);
	int16_t audioFormat = twoBytesToInt(fileData, formatChunkIndex + 8);
	int16_t numChannels = twoBytesToInt(fileData, formatChunkIndex + 10);
	sampleRate = fourBytesToInt(fileData, formatChunkIndex + 12);
	int32_t byteRate = fourBytesToInt(fileData, formatChunkIndex + 16);
	int16_t blockAlign = twoBytesToInt(fileData, formatChunkIndex + 20);
	bitDepth = twoBytesToInt(fileData, formatChunkIndex + 22);
	const auto numBytesPerSample = bitDepth / 8;

	// Must be a PCM format
	if(audioFormat != 1)
	{
		cerr << "Error: Compressed files doesn`t supported." << endl;
		return false;
	}

	// Check number of channels
	if(numChannels < 1 || numChannels > 2)
	{
		cerr << "Error: Supported only mono or stereo files." << endl;
		return false;
	}

	// check header data is consistent
	if ((byteRate != (numChannels * sampleRate * bitDepth) / 8) || (blockAlign != (numChannels * numBytesPerSample)))
	{
		cerr << "Error: the header data in this WAV file seems to be inconsistent" << endl;
		return false;
	}

	// check bit depth is either 8, 16 or 24 bit
	if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24)
	{
		cerr << "Error: this file has a bit depth that is not 8, 16 or 24 bits" << endl;
		return false;
	}

	////////////////////////////////////
	// Data chunk //////////////////////
	string dataChunkId(fileData.begin() + dataChunkIndex, fileData.begin() + dataChunkIndex + 4);
	int32_t dataChunkSize = fourBytesToInt(fileData, dataChunkIndex + 4);

	const int numSamples = dataChunkSize / (numChannels * numBytesPerSample);
	const int samplesStartIndex = dataChunkIndex + 8;

	clearSamples();
	samples.resize(numChannels);

	for (int i = 0; i < numSamples; i++)
	{
		for (int channel = 0; channel < numChannels; channel++)
		{
			int sampleIndex = samplesStartIndex + (blockAlign * i) + channel * numBytesPerSample;

			if (bitDepth == 8)
			{
				T sample = singleByteToSample(fileData[sampleIndex]);
				samples[channel].push_back(sample);
			}
			else if (bitDepth == 16)
			{
				int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex);
				T sample = sixteenBitIntToSample(sampleAsInt);
				samples[channel].push_back(sample);
			}
			else if (bitDepth == 24)
			{
				int32_t sampleAsInt = 0;
				sampleAsInt = (fileData[sampleIndex + 2] << 16) | (fileData[sampleIndex + 1] << 8) | fileData[sampleIndex];

				if (sampleAsInt & 0x800000) //  if the 24th bit is set, this is a negative number in 24-bit world
					sampleAsInt = sampleAsInt | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

				T sample = static_cast<T>(sampleAsInt) / static_cast<T>(8388608.);
				samples[channel].push_back(sample);
			}
			else
			{
				cerr << "Error: Unsupported bit depth: " << bitDepth << endl;
				return false;
			}
		}
	}

	return true;
}

template <typename T>
int WavFile<T>::getNumChannels() const
{
	return static_cast<int>(samples.size());
}

template <typename T>
void WavFile<T>::setNumChannels(int numChannels)
{
	const auto oldNumChannels = getNumChannels();
	const auto oldNumSamplesPerChannel = getNumSamplesPerChannel();

	samples.resize(numChannels);

	if(numChannels > oldNumChannels)
	{
		for(auto i = oldNumChannels; i < numChannels; i++)
		{
			samples[i].resize(oldNumSamplesPerChannel);
			std::fill(samples[i].begin(), samples[i].end(), static_cast<T>(0));
		}
	}
}

template <typename T>
int WavFile<T>::getNumSamplesPerChannel() const
{
	return !samples.empty() ? static_cast<int>(samples[0].size()) : 0;
}

template <typename T>
void WavFile<T>::setNumSamplesPerChannel(int numSamples)
{
	const auto oldNumSamples = getNumSamplesPerChannel();
	for(int i = 0; i < getNumChannels(); i++)
	{
		samples[i].resize(numSamples);
		if (numSamples > oldNumSamples)
			std::fill(samples[i].begin() + oldNumSamples, samples[i].end(), static_cast<T>(0));
	}
}

template <class T>
double WavFile<T>::getLengthInSeconds() const
{
	return static_cast<double>(getNumSamplesPerChannel()) / static_cast<double>(sampleRate);
}

template <typename T>
void WavFile<T>::printSummary() const
{
	cout << "|======================================|" << endl
		 << "| Num Channels: " << getNumChannels() << endl
		 << "| Num Samples Per Channel: " << getNumSamplesPerChannel() << endl
		 << "| Sample Rate: " << sampleRate << endl
		 << "| Bit Depth: " << bitDepth << endl
		 << "| Length in Seconds: " << getLengthInSeconds() << endl
		 << "|======================================|" << endl;
}

template <typename T>
void WavFile<T>::clearSamples()
{
	for (auto& channel : samples)
		channel.clear();
	samples.clear();
}

template <typename T>
int16_t WavFile<T>::twoBytesToInt(FileData& source, int startIndex)
{
	return (source[startIndex + 1] << 8) | source[startIndex];
}

template <typename T>
int32_t WavFile<T>::fourBytesToInt(FileData& source, int startIndex)
{
	return (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) | source[startIndex];
}

template <typename T>
int WavFile<T>::getIndexOfStr(FileData source, std::string str) const
{
	int idx = -1;
	const size_t str_len = str.length();

	for(size_t i = 0; i < source.size() - str_len; i++)
	{
		string part(source.begin() + i, source.begin() + i + str_len);
		if(part == str)
		{
			idx = i;
			break;
		}
	}

	return idx;
}

template <class T>
T WavFile<T>::sixteenBitIntToSample(int16_t sample)
{
	return static_cast<T> (sample) / static_cast<T> (32768.);
}

template <class T>
int16_t WavFile<T>::sampleToSixteenBitInt(T sample)
{
	sample = clamp(sample, -1., 1.);
	return static_cast<int16_t> (sample * 32767.);
}

template <class T>
uint8_t WavFile<T>::sampleToSingleByte(T sample)
{
	sample = clamp(sample, -1., 1.);
	sample = (sample + 1.) / 2.;
	return static_cast<uint8_t> (sample * 255.);
}

template <class T>
T WavFile<T>::singleByteToSample(uint8_t sample)
{
	return static_cast<T> (sample - 128) / static_cast<T> (128.);
}

template <typename T>
T WavFile<T>::clamp(T value, T minValue, T maxValue)
{
	value = std::min(value, maxValue);
	value = std::max(value, minValue);
	return value;
}

template class WavFile<float>;
template class WavFile<double>;
