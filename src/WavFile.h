#pragma once
#include <vector>
#include <string>

template<typename T>
class WavFile
{
public:
	// Outer container is channels, inner is samples.
	typedef std::vector<std::vector<T>> AudioData;

	// Binary file data
	typedef std::vector<uint8_t> FileData;

	WavFile();
	WavFile(uint32_t sample_rate, int bit_depth, AudioData samples);
	WavFile(const WavFile& other) = default;
	WavFile(const WavFile&& other) noexcept;
	~WavFile() = default;

	WavFile& operator=(const WavFile& other) = default;
	WavFile& operator=(WavFile&& other) = default;
	
	/**
	 * \brief Save wave file
	 * \param filename File to Save
	 * \return  true, if saving was successful, otherwise false
	 */
	bool Save(const std::string& filename);

	/**
	 * \brief Load wave file
	 * \param filename File to load
	 * \return true, if loading was successful, otherwise false
	 */
	bool Load(const std::string& filename);

	[[nodiscard]] size_t GetNumChannels() const;
	void SetNumChannels(size_t num_channels);

	[[nodiscard]] size_t GetNumSamplesPerChannel() const;
	void SetNumSamplesPerChannel(size_t num_samples);

	[[nodiscard]] double GetLengthInSeconds() const;

	[[nodiscard]] bool IsMono() const;
	[[nodiscard]] bool IsStereo() const;
	[[nodiscard]] bool IsMultiTrack() const;

	/**
	 * \brief Prints wave file summary to standart output
	 */
	void PrintSummary() const;
	
	// ReSharper disable CppInconsistentNaming
	uint32_t sampleRate;
	int bitDepth;
	AudioData samples;
	// ReSharper restore CppInconsistentNaming

private:
	void ClearSamples();

	static int16_t TwoBytesToInt(FileData& source, size_t start_index);
	static int32_t FourBytesToInt(FileData& source, size_t start_index);

	/**
	 * \brief Find the index of the first occurrence of the string
	 * \param source source data
	 * \param str string to find for
	 * \return index of the first occurence if founded, otherwise `std::string::npos`
	 */
	[[nodiscard]] static size_t GetIndexOfStr(const FileData& source, std::string_view str);

	static void WriteStringToFileData(FileData& data, const std::string& str);
	static void WriteInt16ToFileData(FileData& data, int16_t i);
	static void WriteInt32ToFileData(FileData& data, int32_t i);
	static bool WriteDataToFile(FileData& data, const std::string& filename);

	static T SixteenBitIntToSample(int16_t sample);
	static int16_t SampleToSixteenBitInt(T sample);

	static uint8_t SampleToSingleByte(T sample);
	static T SingleByteToSample(uint8_t sample);
};
