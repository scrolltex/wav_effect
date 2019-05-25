#include <iostream>
#include "ApplyEffectMenu.h"
#include "WavManager.h"
#include "../Effects.h"

using namespace std;
using namespace effects;

ApplyEffectMenu::ApplyEffectMenu(MenuStack* menuStack) : MenuBase(menuStack)
{
	m_title = "Apply effect";
	m_menuItems = {
		"Back",
		"Mono -> Stereo",
		"Reverse",
		"Volume",
		"Reverberation",
		"Rotating",
		"Fade",
		"Tremolo",
		"Delay",
		"Compressor",
		"Distortion"
	};
}

void ApplyEffectMenu::onSelect()
{
	// Back
	if (selectedIndex == 0)
	{
		back();
		return;
	}

	system("cls");
	switch (selectedIndex)
	{
		case 1: // Mono -> Stereo
			monoToStereo();
			break;

		case 2: // Reverse
			reverse();
			break;

		case 3: // Volume
			volume();
			break;

		case 4: // Reverberation
			reverberation();
			break;

		case 5: // Rotating
			rotating();
			break;

		case 6: // Fade
			fade();
			break;

		case 7: // Tremolo
			tremolo();
			break;

		case 8: // Delay
			delay();
			break;

		case 9: // Compressor
			compressor();
			break;

		case 10: // Distortion
			distortion();
			break;

		default: 
			throw out_of_range("Effect idx out-of-range: " + to_string(selectedIndex));
	}

	m_wm.isFileUnsaved = true;
	waitForEscape();
}

void ApplyEffectMenu::monoToStereo() const
{
	if (!m_wm.wav.isMono())
	{
		cout << "File not in mono. Aborting." << endl;
		return;
	}

	cout << "Converting mono to stereo...";
	effects::monoToStereo(m_wm.wav);
	cout << "Done" << endl;
}

void ApplyEffectMenu::reverse() const
{
	cout << "Reversing audio...";
	applyReverse(m_wm.wav);
	cout << "Done" << endl;
}

void ApplyEffectMenu::volume() const
{
	cout << "Enter volume for increasing in dB: ";
	const auto volume = readValue<float>();

	cout << "Applying volume...";
	applyVolume(m_wm.wav, volume);
	cout << "Done" << endl;
}

void ApplyEffectMenu::reverberation() const
{
	cout << "Applying reverberation...";
	applyReverberation(m_wm.wav);
	cout << "Done" << endl;
}

void ApplyEffectMenu::rotating() const
{
	if (m_wm.wav.isMono())
	{
		if (!dialog("File must be in stereo. Convert into it?"))
		{
			cout << "Aborting." << endl;
			return;
		}

		monoToStereo();
	}

	cout << "Enter rotating rate in seconds: ";
	const auto rate = readValue<float>(&greaterThanZero);

	cout << "Applying rotating...";
	applyRotatingStereo(m_wm.wav, rate);
	cout << "Done" << endl;
}

void ApplyEffectMenu::fade() const
{
	// Enter fade type
	cout << "Fade type: 1 - fade in, 0 - fade out." << endl
		<< "Enter fade type: ";
	const bool isFadeIn = readValue<int>([](auto value) { return value == 0 || value == 1; }) == 1;

	// Enter time
	cout << "Enter fade time in seconds: ";
	const auto fadeTime = readValue<float>([&](auto value) { return value > 0 && value < m_wm.wav.getLengthInSeconds(); });

	// Enter curve
	cout << "Curve type:" << endl
		<< " 1 - Linear" << endl
		<< " 2 - Logarithmic" << endl
		<< " 3 - Sine" << endl;
	cout << "Enter curve type: ";
	const auto curveType = static_cast<CurveType>(readValue<int>([](auto value) { return value >= 1 && value <= 3; }));

	// Apply fade
	cout << "Applying fade...";
	if (isFadeIn)
		applyFadeIn(m_wm.wav, fadeTime, curveType);
	else
		applyFadeOut(m_wm.wav, fadeTime, curveType);
	cout << "Done" << endl;
}

void ApplyEffectMenu::tremolo() const
{
	cout << "Enter tremolo frequency in Hz: ";
	const auto freq = readValue<float>(&greaterThanZero);

	cout << "Enter dry signal percent (0..1): ";
	const auto dry = readValue<float>(&isNormalizedValue);

	cout << "Applying tremolo...";
	applyTremolo(m_wm.wav, freq, dry, 1.f - dry);
	cout << "Done" << endl;
}

void ApplyEffectMenu::delay() const
{
	cout << "Enter delay time in ms: ";
	const auto delayTime = readValue<float>(&greaterThanZero);

	cout << "Enter decay: ";
	const auto decay = readValue<float>(&isNormalizedValue);

	cout << "Enter channel num, or 0 for applying on all channels: ";
	const auto channelIdx = readValue<size_t>([this](auto value) {
		return value >= 0 && value <= m_wm.wav.getNumChannels();
	}) - 1;

	cout << "Applying delay...";
	if (channelIdx == 0)
		applyDelay(m_wm.wav, delayTime, decay);
	else
		applyDelay(m_wm.wav, channelIdx - 1, delayTime, decay);
	cout << "Done" << endl;
}

void ApplyEffectMenu::compressor() const
{
	cout << "Enter threshold (in dB) for compressor: ";
	const auto threshold = readValue<float>([](auto value) { return value <= 0; });

	cout << "Enter ratio:";
	const auto ratio = readValue<float>([](auto value) { return value >= 1; });

	const bool downward = dialog("Compress downward?");

	cout << "Applying compressor...";
	applyCompressor(m_wm.wav, threshold, ratio, downward);
	cout << "Done" << endl;
}

void ApplyEffectMenu::distortion() const
{
	cout << "Enter distortion drive (0..1): ";
	const auto drive = readValue<float>(&isNormalizedValue);

	cout << "Enter blend (0..1): ";
	const auto blend = readValue<float>(&isNormalizedValue);

	cout << "Applying distortion...";
	applyDistortion(m_wm.wav, drive, blend);
	cout << "Done" << endl;
}
