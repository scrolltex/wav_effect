#pragma once
#include <filesystem>
#include "../WavFile.h"

class WavManager
{
public:
	~WavManager() = default;

	WavManager(const WavManager&) = default;
	WavManager(WavManager&&) = default;
	WavManager& operator=(const WavManager&) = default;
	WavManager& operator=(WavManager&&) = default;

	static WavManager& get()
	{
		static WavManager m_instance;
		return m_instance;
	}

	WavFile<float> wav;
	std::filesystem::path filepath;
	std::filesystem::path out_filepath;
	bool isFileUnsaved = false;

private:
	WavManager() = default;
};
