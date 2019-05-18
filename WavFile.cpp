#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <utility>
#include "WavFile.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::clamp;

template <typename T>
WavFile<T>::WavFile()
{
	bitDepth = 16;
	sampleRate = 44100;
	samples.resize(1);
	samples[0].resize(0);
}

template <typename T>
WavFile<T>::WavFile(uint32_t sampleRate, int bitDepth, AudioData samples)
	: sampleRate(sampleRate), bitDepth(bitDepth), samples(std::move(samples))
{
	
}

template <typename T>
WavFile<T>::WavFile(const WavFile&& other) noexcept
{
	sampleRate = other.sampleRate;
	bitDepth = other.bitDepth;
	samples = other.samples;
}

template <typename T>
bool WavFile<T>::save(const std::string& filename)
{
	FileData fileData;
	const int32_t dataChunkSize = getNumSamplesPerChannel() * (getNumChannels() * bitDepth / 8);

	////////////////////////////////////////////////////////////////////////////
	// Header chunk ////////////////////////////////////////////////////////////
	writeStringToFileData(fileData, "RIFF");

	// The file size in bytes is the header chunk size (4, not counting RIFF and WAVE) + the format
	// chunk size (24) + the metadata part of the data chunk plus the actual data chunk size
	const int32_t fileSizeInBytes = 4 + 24 + 8 + dataChunkSize;
	writeInt32ToFileData(fileData, fileSizeInBytes);

	writeStringToFileData(fileData, "WAVE");

	////////////////////////////////////////////////////////////////////////////
	// Format chunk ////////////////////////////////////////////////////////////
	writeStringToFileData(fileData, "fmt ");
	writeInt32ToFileData(fileData, 16); // format chunk size (16 for PCM)
	writeInt16ToFileData(fileData, 1); // audio format = 1
	writeInt16ToFileData(fileData, static_cast<int16_t>(getNumChannels())); // num channels
	writeInt32ToFileData(fileData, static_cast<int32_t>(sampleRate)); // sample rate

	const int32_t numBytesPerSecond = static_cast<int32_t>(getNumChannels() * sampleRate * bitDepth / 8);
	writeInt32ToFileData(fileData, numBytesPerSecond);

	const int16_t numBytesPerBlock = getNumChannels() * (bitDepth / 8);
	writeInt16ToFileData(fileData, numBytesPerBlock);

	writeInt16ToFileData(fileData, static_cast<int16_t>(bitDepth));
	
	////////////////////////////////////////////////////////////////////////////
	// Data chunk //////////////////////////////////////////////////////////////
	writeStringToFileData(fileData, "data");
	writeInt32ToFileData(fileData, dataChunkSize);

	for (size_t i = 0; i < getNumSamplesPerChannel(); i++)
	{
		for (size_t channel = 0; channel < getNumChannels(); channel++)
		{
			if (bitDepth == 8)
			{
				uint8_t byte = sampleToSingleByte(samples[channel][i]);
				fileData.push_back(byte);
			}
			else if (bitDepth == 16)
			{
				const int16_t sampleAsInt = sampleToSixteenBitInt(samples[channel][i]);
				writeInt16ToFileData(fileData, sampleAsInt);
			}
			else if (bitDepth == 24)
			{
				const auto sampleAsIntAgain = static_cast<int32_t>(samples[channel][i] * static_cast<T>(1 << 23));

				uint8_t bytes[3];
				bytes[2] = static_cast<uint8_t>(sampleAsIntAgain >> 16) & 0xFF;
				bytes[1] = static_cast<uint8_t>(sampleAsIntAgain >> 8) & 0xFF;
				bytes[0] = static_cast<uint8_t>(sampleAsIntAgain) & 0xFF;

				fileData.push_back(bytes[0]);
				fileData.push_back(bytes[1]);
				fileData.push_back(bytes[2]);
			}
			else if (bitDepth == 32)
			{
				const auto sampleAsIntAgain = static_cast<int32_t>(samples[channel][i] * std::numeric_limits<int32_t>::max());

				uint8_t bytes[4];
				bytes[3] = static_cast<uint8_t>(sampleAsIntAgain >> 24) & 0xFF;
				bytes[2] = static_cast<uint8_t>(sampleAsIntAgain >> 16) & 0xFF;
				bytes[1] = static_cast<uint8_t>(sampleAsIntAgain >> 8) & 0xFF;
				bytes[0] = static_cast<uint8_t>(sampleAsIntAgain >> 0) & 0xFF;

				fileData.push_back(bytes[0]);
				fileData.push_back(bytes[1]);
				fileData.push_back(bytes[2]);
				fileData.push_back(bytes[3]);
			}
			else
			{
				cerr << "Error: Trying to write a file with unsupported bit depth" << endl;
				return false;
			}
		}
	}

	// check that the various sizes we put in the metadata are correct
	if (static_cast<size_t>(fileSizeInBytes) != (fileData.size() - 8) || 
		static_cast<size_t>(dataChunkSize)   != (getNumSamplesPerChannel() * getNumChannels() * (bitDepth / 8)))
	{
		cerr << "Error: couldn't save file to " << filename << endl;
		return false;
	}

	// try to write the file
	return writeDataToFile(fileData, filename);
}

template <typename T>
bool WavFile<T>::load(const std::string& filename)
{
	// Open file stream
	std::ifstream file(filename, std::ios::binary);

	if (!file.good())
	{
		cerr << "Error: Can`t open file: " << filename << endl;
		return false;
	}

	auto const start_pos = file.tellg();
	file.ignore(std::numeric_limits<std::streamsize>::max());
	auto const char_count = file.gcount();
	file.seekg(start_pos);
	FileData fileData(char_count);
	file.read(reinterpret_cast<char*>(&fileData[0]), fileData.size());

	////////////////////////////////////////////////////////////////////////////
	// Read header chunk ///////////////////////////////////////////////////////
	const string headerChunkId(fileData.begin(), fileData.begin() + 4);
	const string format(fileData.begin() + 8, fileData.begin() + 12);

	// Find indexies of format and data chunks
	const auto formatChunkIndex = getIndexOfStr(fileData, "fmt"); 
	const auto dataChunkIndex = getIndexOfStr(fileData, "data");

	// Header must start with RIFF, format must be a WAVE, and file must contains format and data chunks
	if (headerChunkId != "RIFF" || format != "WAVE" || formatChunkIndex == string::npos || dataChunkIndex == string::npos)
	{
		cerr << "Error: Invalid .wav file." << endl;
		return false;
	}

	////////////////////////////////////////////////////////////////////////////
	// Format chunk ////////////////////////////////////////////////////////////
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
	if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32)
	{
		cerr << "Error: this file has a bit depth that is not 8, 16, 24 or 32 bits" << endl;
		return false;
	}

	////////////////////////////////////////////////////////////////////////////
	// Data chunk //////////////////////////////////////////////////////////////
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

				if (sampleAsInt & (1 << 23)) //  if the 24th bit is set, this is a negative number in 24-bit world
					sampleAsInt = sampleAsInt | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

				T sample = static_cast<T>(sampleAsInt) / static_cast<T>(1 << 23);
				samples[channel].push_back(sample);
			}
			else if (bitDepth == 32)
			{
				int32_t sampleAsInt = fourBytesToInt(fileData, sampleIndex);
				T sample = static_cast<T>(sampleAsInt) / static_cast<T>(std::numeric_limits<std::int32_t>::max());
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
size_t WavFile<T>::getNumChannels() const
{
	return samples.size();
}

template <typename T>
void WavFile<T>::setNumChannels(size_t numChannels)
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
size_t WavFile<T>::getNumSamplesPerChannel() const
{
	return !samples.empty() ? static_cast<int>(samples[0].size()) : 0;
}

template <typename T>
void WavFile<T>::setNumSamplesPerChannel(size_t numSamples)
{
	const auto oldNumSamples = getNumSamplesPerChannel();
	for(size_t i = 0; i < getNumChannels(); i++)
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
bool WavFile<T>::isMono() const
{
	return samples.size() == 1;
}

template <typename T>
bool WavFile<T>::isStereo() const
{
	return samples.size() == 2;	
}

template <typename T>
bool WavFile<T>::isMultiTrack() const
{
	return samples.size() > 2;
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

template <class T>
void WavFile<T>::writeStringToFileData(FileData& fileData, const std::string& str)
{
    for (auto i : str)
	    fileData.push_back (static_cast<uint8_t>(i));
}

template <class T>
void WavFile<T>::writeInt16ToFileData(FileData& fileData, int16_t i)
{
	uint8_t bytes[2];
	bytes[1] = (i >> 8) & 0xFF;
	bytes[0] = i & 0xFF;

	fileData.push_back(bytes[0]);
	fileData.push_back(bytes[1]);
}

template <class T>
void WavFile<T>::writeInt32ToFileData(FileData& fileData, int32_t i)
{
    uint8_t bytes[4];
    bytes[3] = (i >> 24) & 0xFF;
    bytes[2] = (i >> 16) & 0xFF;
    bytes[1] = (i >> 8) & 0xFF;
    bytes[0] = i & 0xFF;
    
    for(auto byte : bytes)
	    fileData.push_back(byte);
}

template <class T>
bool WavFile<T>::writeDataToFile(FileData& fileData, const std::string& filename)
{
	std::ofstream outputFile(filename, std::ios::binary);

	if (outputFile.is_open())
	{
		std::copy(fileData.begin(), fileData.end(), std::ostreambuf_iterator<char>(outputFile));
		outputFile.close();
		return true;
	}

	return false;
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
size_t WavFile<T>::getIndexOfStr(const FileData& source, std::string_view str)
{
	size_t idx = std::string::npos;
	const size_t str_len = str.length();

	for(size_t i = 0; i < source.size() - str_len; i++)
	{
		std::string part(source.begin() + i, source.begin() + i + str_len);
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
	sample = clamp<T>(sample, -1., 1.);
	return static_cast<int16_t> (sample * 32767.);
}

template <class T>
uint8_t WavFile<T>::sampleToSingleByte(T sample)
{
	sample = clamp<T>(sample, -1., 1.);
	sample = (sample + 1.) / 2.;
	return static_cast<uint8_t> (sample * 255.);
}

template <class T>
T WavFile<T>::singleByteToSample(uint8_t sample)
{
	return static_cast<T> (sample - 128) / static_cast<T> (128.);
}

template class WavFile<float>;
template class WavFile<double>;
