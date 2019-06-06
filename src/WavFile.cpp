#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <utility>
#include "WavFile.h"

using namespace std;

template <typename T>
WavFile<T>::WavFile()
{
	bitDepth = 16;
	sampleRate = 44100;
	samples.resize(1);
	samples[0].resize(0);
}

template <typename T>
WavFile<T>::WavFile(uint32_t sample_rate, int bit_depth, AudioData samples)
	: sampleRate(sample_rate), bitDepth(bit_depth), samples(std::move(samples))
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
bool WavFile<T>::Save(const std::string& filename)
{
	FileData data;
	const int32_t data_chunk_size = GetNumSamplesPerChannel() * (GetNumChannels() * bitDepth / 8);

	////////////////////////////////////////////////////////////////////////////
	// Header chunk ////////////////////////////////////////////////////////////
	WriteStringToFileData(data, "RIFF");

	// The file size in bytes is the header chunk size (4, not counting RIFF and WAVE) + the format
	// chunk size (24) + the metadata part of the data chunk plus the actual data chunk size
	const int32_t file_size_in_bytes = 4 + 24 + 8 + data_chunk_size;
	WriteInt32ToFileData(data, file_size_in_bytes);

	WriteStringToFileData(data, "WAVE");

	////////////////////////////////////////////////////////////////////////////
	// Format chunk ////////////////////////////////////////////////////////////
	WriteStringToFileData(data, "fmt ");
	WriteInt32ToFileData(data, 16); // format chunk size (16 for PCM)
	WriteInt16ToFileData(data, 1); // audio format = 1
	WriteInt16ToFileData(data, static_cast<int16_t>(GetNumChannels())); // num channels
	WriteInt32ToFileData(data, static_cast<int32_t>(sampleRate)); // sample rate

	const int32_t num_bytes_per_second = static_cast<int32_t>(GetNumChannels() * sampleRate * bitDepth / 8);
	WriteInt32ToFileData(data, num_bytes_per_second);

	const int16_t num_bytes_per_block = GetNumChannels() * (bitDepth / 8);
	WriteInt16ToFileData(data, num_bytes_per_block);

	WriteInt16ToFileData(data, static_cast<int16_t>(bitDepth));
	
	////////////////////////////////////////////////////////////////////////////
	// Data chunk //////////////////////////////////////////////////////////////
	WriteStringToFileData(data, "data");
	WriteInt32ToFileData(data, data_chunk_size);

	for (size_t i = 0; i < GetNumSamplesPerChannel(); i++)
	{
		for (size_t channel = 0; channel < GetNumChannels(); channel++)
		{
			if (bitDepth == 8)
			{
				uint8_t byte = SampleToSingleByte(samples[channel][i]);
				data.push_back(byte);
			}
			else if (bitDepth == 16)
			{
				const int16_t sample_as_int = SampleToSixteenBitInt(samples[channel][i]);
				WriteInt16ToFileData(data, sample_as_int);
			}
			else if (bitDepth == 24)
			{
				const auto sample_as_int_again = static_cast<int32_t>(samples[channel][i] * static_cast<T>(1 << 23));

				uint8_t bytes[3];
				bytes[2] = static_cast<uint8_t>(sample_as_int_again >> 16) & 0xFF;
				bytes[1] = static_cast<uint8_t>(sample_as_int_again >> 8) & 0xFF;
				bytes[0] = static_cast<uint8_t>(sample_as_int_again) & 0xFF;

				data.push_back(bytes[0]);
				data.push_back(bytes[1]);
				data.push_back(bytes[2]);
			}
			else if (bitDepth == 32)
			{
				const auto sample_as_int_again = static_cast<int32_t>(samples[channel][i] * std::numeric_limits<int32_t>::max());

				uint8_t bytes[4];
				bytes[3] = static_cast<uint8_t>(sample_as_int_again >> 24) & 0xFF;
				bytes[2] = static_cast<uint8_t>(sample_as_int_again >> 16) & 0xFF;
				bytes[1] = static_cast<uint8_t>(sample_as_int_again >> 8) & 0xFF;
				bytes[0] = static_cast<uint8_t>(sample_as_int_again >> 0) & 0xFF;

				data.push_back(bytes[0]);
				data.push_back(bytes[1]);
				data.push_back(bytes[2]);
				data.push_back(bytes[3]);
			}
			else
			{
				cerr << "Error: Trying to write a file with unsupported bit depth" << endl;
				return false;
			}
		}
	}

	// check that the various sizes we put in the metadata are correct
	if (static_cast<size_t>(file_size_in_bytes) != (data.size() - 8) || 
		static_cast<size_t>(data_chunk_size)   != (GetNumSamplesPerChannel() * GetNumChannels() * (bitDepth / 8)))
	{
		cerr << "Error: couldn't Save file to " << filename << endl;
		return false;
	}

	// try to write the file
	return WriteDataToFile(data, filename);
}

template <typename T>
bool WavFile<T>::Load(const std::string& filename)
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
	FileData data(char_count);
	file.read(reinterpret_cast<char*>(&data[0]), data.size());

	////////////////////////////////////////////////////////////////////////////
	// Read header chunk ///////////////////////////////////////////////////////
	const string header_chunk_id(data.begin(), data.begin() + 4);
	const string format(data.begin() + 8, data.begin() + 12);

	// Find indexies of format and data chunks
	const auto format_chunk_index = GetIndexOfStr(data, "fmt"); 
	const auto data_chunk_index = GetIndexOfStr(data, "data");

	// Header must start with RIFF, format must be a WAVE, and file must contains format and data chunks
	if (header_chunk_id != "RIFF" || format != "WAVE" || 
		format_chunk_index == string::npos || data_chunk_index == string::npos)
	{
		cerr << "Error: Invalid .wav file." << endl;
		return false;
	}

	////////////////////////////////////////////////////////////////////////////
	// Format chunk ////////////////////////////////////////////////////////////
	string format_chunk_id(data.begin() + format_chunk_index, data.begin() + format_chunk_index + 4);
	int16_t audio_format = TwoBytesToInt(data, format_chunk_index + 8);
	int16_t num_channels = TwoBytesToInt(data, format_chunk_index + 10);
	sampleRate = FourBytesToInt(data, format_chunk_index + 12);
	int32_t byte_rate = FourBytesToInt(data, format_chunk_index + 16);
	int16_t block_align = TwoBytesToInt(data, format_chunk_index + 20);
	bitDepth = TwoBytesToInt(data, format_chunk_index + 22);
	const auto num_bytes_per_sample = bitDepth / 8;

	// Must be a PCM format
	if(audio_format != 1)
	{
		cerr << "Error: Compressed files doesn`t supported." << endl;
		return false;
	}

	// Check number of channels
	if(num_channels < 1 || num_channels > 2)
	{
		cerr << "Error: Supported only mono or stereo files." << endl;
		return false;
	}

	// check header data is consistent
	if (byte_rate != num_channels * sampleRate * bitDepth / 8 || 
		block_align != num_channels * num_bytes_per_sample)
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
	string data_chunk_id(data.begin() + data_chunk_index, data.begin() + data_chunk_index + 4);
	int32_t data_chunk_size = FourBytesToInt(data, data_chunk_index + 4);

	const int num_samples = data_chunk_size / (num_channels * num_bytes_per_sample);
	const int samples_start_index = data_chunk_index + 8;

	ClearSamples();
	samples.resize(num_channels);

	for (int i = 0; i < num_samples; i++)
	{
		for (int channel = 0; channel < num_channels; channel++)
		{
			int sample_index = samples_start_index + (block_align * i) + channel * num_bytes_per_sample;

			if (bitDepth == 8)
			{
				T sample = SingleByteToSample(data[sample_index]);
				samples[channel].push_back(sample);
			}
			else if (bitDepth == 16)
			{
				int16_t sample_as_int = TwoBytesToInt(data, sample_index);
				T sample = SixteenBitIntToSample(sample_as_int);
				samples[channel].push_back(sample);
			}
			else if (bitDepth == 24)
			{
				int32_t sample_as_int = 0;
				sample_as_int = (data[sample_index + 2] << 16) | (data[sample_index + 1] << 8) | data[sample_index];

				if (sample_as_int & (1 << 23)) //  if the 24th bit is set, this is a negative number in 24-bit world
					sample_as_int = sample_as_int | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

				T sample = static_cast<T>(sample_as_int) / static_cast<T>(1 << 23);
				samples[channel].push_back(sample);
			}
			else if (bitDepth == 32)
			{
				int32_t sample_as_int = FourBytesToInt(data, sample_index);
				T sample = static_cast<T>(sample_as_int) / static_cast<T>(std::numeric_limits<std::int32_t>::max());
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
size_t WavFile<T>::GetNumChannels() const
{
	return samples.size();
}

template <typename T>
void WavFile<T>::SetNumChannels(size_t num_channels)
{
	const auto old_num_channels = GetNumChannels();
	const auto old_num_samples_per_channel = GetNumSamplesPerChannel();

	samples.resize(num_channels);

	if(num_channels > old_num_channels)
	{
		for(auto i = old_num_channels; i < num_channels; i++)
		{
			samples[i].resize(old_num_samples_per_channel);
			std::fill(samples[i].begin(), samples[i].end(), static_cast<T>(0));
		}
	}
}

template <typename T>
size_t WavFile<T>::GetNumSamplesPerChannel() const
{
	return !samples.empty() ? static_cast<int>(samples[0].size()) : 0;
}

template <typename T>
void WavFile<T>::SetNumSamplesPerChannel(size_t num_samples)
{
	const auto old_num_samples = GetNumSamplesPerChannel();
	for(size_t i = 0; i < GetNumChannels(); i++)
	{
		samples[i].resize(num_samples);
		if (num_samples > old_num_samples)
			std::fill(samples[i].begin() + old_num_samples, samples[i].end(), static_cast<T>(0));
	}
}

template <class T>
double WavFile<T>::GetLengthInSeconds() const
{
	return static_cast<double>(GetNumSamplesPerChannel()) / static_cast<double>(sampleRate);
}

template <typename T>
bool WavFile<T>::IsMono() const
{
	return samples.size() == 1;
}

template <typename T>
bool WavFile<T>::IsStereo() const
{
	return samples.size() == 2;	
}

template <typename T>
bool WavFile<T>::IsMultiTrack() const
{
	return samples.size() > 2;
}

template <typename T>
void WavFile<T>::PrintSummary() const
{
	cout << "|======================================|" << endl
		 << "| Num Channels: " << GetNumChannels() << endl
		 << "| Num Samples Per Channel: " << GetNumSamplesPerChannel() << endl
		 << "| Sample Rate: " << sampleRate << endl
		 << "| Bit Depth: " << bitDepth << endl
		 << "| Length in Seconds: " << GetLengthInSeconds() << endl
		 << "|======================================|" << endl;
}

template <class T>
void WavFile<T>::WriteStringToFileData(FileData& data, const std::string& str)
{
    for (auto i : str)
	    data.push_back (static_cast<uint8_t>(i));
}

template <class T>
void WavFile<T>::WriteInt16ToFileData(FileData& data, int16_t i)
{
	uint8_t bytes[2];
	bytes[1] = (i >> 8) & 0xFF;
	bytes[0] = i & 0xFF;

	data.push_back(bytes[0]);
	data.push_back(bytes[1]);
}

template <class T>
void WavFile<T>::WriteInt32ToFileData(FileData& data, int32_t i)
{
    uint8_t bytes[4];
    bytes[3] = (i >> 24) & 0xFF;
    bytes[2] = (i >> 16) & 0xFF;
    bytes[1] = (i >> 8) & 0xFF;
    bytes[0] = i & 0xFF;
    
    for(auto byte : bytes)
	    data.push_back(byte);
}

template <class T>
bool WavFile<T>::WriteDataToFile(FileData& data, const std::string& filename)
{
	std::ofstream file(filename, std::ios::binary);

	if (file.is_open())
	{
		std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(file));
		file.close();
		return true;
	}

	return false;
}

template <typename T>
void WavFile<T>::ClearSamples()
{
	for (auto& channel : samples)
		channel.clear();
	samples.clear();
}

template <typename T>
int16_t WavFile<T>::TwoBytesToInt(FileData& source, size_t start_index)
{
	return (source[start_index + 1] << 8) | source[start_index];
}

template <typename T>
int32_t WavFile<T>::FourBytesToInt(FileData& source, size_t start_index)
{
	return (source[start_index + 3] << 24) | (source[start_index + 2] << 16) | (source[start_index + 1] << 8) | source[start_index];
}

template <typename T>
size_t WavFile<T>::GetIndexOfStr(const FileData& source, std::string_view str)
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
T WavFile<T>::SixteenBitIntToSample(int16_t sample)
{
	return static_cast<T> (sample) / static_cast<T> (32768.);
}

template <class T>
int16_t WavFile<T>::SampleToSixteenBitInt(T sample)
{
	sample = clamp<T>(sample, -1., 1.);
	return static_cast<int16_t> (sample * 32767.);
}

template <class T>
uint8_t WavFile<T>::SampleToSingleByte(T sample)
{
	sample = clamp<T>(sample, -1., 1.);
	sample = (sample + 1.) / 2.;
	return static_cast<uint8_t> (sample * 255.);
}

template <class T>
T WavFile<T>::SingleByteToSample(uint8_t sample)
{
	return static_cast<T> (sample - 128) / static_cast<T> (128.);
}

template class WavFile<float>;
template class WavFile<double>;
