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
	WavFile(uint32_t sampleRate, int bitDepth, AudioData samples);
	WavFile(const WavFile& other) = default;
	WavFile(const WavFile&& other) noexcept;
	~WavFile() = default;

	WavFile& operator=(const WavFile& other) = default;
	WavFile& operator=(WavFile&& other) = default;
	
	/**
	 * \brief Save wave file
	 * \param filename File to save
	 * \return  true, if saving was successful, otherwise false
	 */
	bool save(const std::string& filename);

	/**
	 * \brief Load wave file
	 * \param filename File to load
	 * \return true, if loading was successful, otherwise false
	 */
	bool load(const std::string& filename);

	[[nodiscard]] size_t getNumChannels() const;
	void setNumChannels(size_t numChannels);

	[[nodiscard]] size_t getNumSamplesPerChannel() const;
	void setNumSamplesPerChannel(size_t numSamples);

	[[nodiscard]] double getLengthInSeconds() const;

	[[nodiscard]] bool isMono() const;
	[[nodiscard]] bool isStereo() const;
	[[nodiscard]] bool isMultiTrack() const;

	/**
	 * \brief Prints wave file summary to standart output
	 */
	void printSummary() const;
	
	uint32_t sampleRate;
	int bitDepth;
	AudioData samples;

private:
	void clearSamples();

	static int16_t twoBytesToInt(FileData& source, int startIndex);
	static int32_t fourBytesToInt(FileData& source, int startIndex);

	/**
	 * \brief Find the index of the first occurrence of the string
	 * \param source source data
	 * \param str string to find for
	 * \return index of the first occurence if founded, otherwise `std::string::npos`
	 */
	[[nodiscard]] static size_t getIndexOfStr(const FileData& source, std::string_view str);

	static void writeStringToFileData(FileData& fileData, const std::string& str);
	static void writeInt16ToFileData(FileData& fileData, int16_t i);
	static void writeInt32ToFileData(FileData& fileData, int32_t i);
	static bool writeDataToFile(FileData& fileData, const std::string& filename);

	static T sixteenBitIntToSample(int16_t sample);
	static int16_t sampleToSixteenBitInt(T sample);

	static uint8_t sampleToSingleByte(T sample);
	static T singleByteToSample(uint8_t sample);
};