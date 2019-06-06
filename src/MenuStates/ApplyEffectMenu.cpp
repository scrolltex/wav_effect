#include <iostream>
#include "ApplyEffectMenu.h"
#include "../Effects.h"

using namespace std;
using namespace effects;

ApplyEffectMenu::ApplyEffectMenu(MenuStack* menu_stack) : MenuStateBase(menu_stack)
{
	title_ = "Apply effect";
	menu_items_ = {
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

void ApplyEffectMenu::OnSelect()
{
	// Back
	if (selected_index_ == 0)
	{
		Back();
		return;
	}

	system("cls");
	switch (selected_index_)
	{
		case 1: // Mono -> Stereo
			mono_to_stereo();
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
			throw out_of_range("Effect idx out-of-range: " + to_string(selected_index_));
	}

	wm_.isFileUnsaved = true;
	WaitForEscape();
}

void ApplyEffectMenu::mono_to_stereo() const
{
	if (!wm_.wav.IsMono())
	{
		cout << "File not in mono. Aborting." << endl;
		return;
	}

	cout << "Converting mono to stereo...";
	MonoToStereo(wm_.wav);
	cout << "Done" << endl;
}

void ApplyEffectMenu::reverse() const
{
	cout << "Reversing audio...";
	ApplyReverse(wm_.wav);
	cout << "Done" << endl;
}

void ApplyEffectMenu::volume() const
{
	cout << "Enter volume for increasing in dB: ";
	const auto volume = ReadValue<float>();

	cout << "Applying volume...";
	ApplyVolume(wm_.wav, volume);
	cout << "Done" << endl;
}

void ApplyEffectMenu::reverberation() const
{
	cout << "Applying reverberation...";
	ApplyReverberation(wm_.wav);
	cout << "Done" << endl;
}

void ApplyEffectMenu::rotating() const
{
	if (wm_.wav.IsMono())
	{
		if (!Ask("File must be in stereo. Convert into it?"))
		{
			cout << "Aborting." << endl;
			return;
		}

		mono_to_stereo();
	}

	cout << "Enter rotating rate in seconds: ";
	const auto rate = ReadValue<float>(&GreaterThanZero);

	cout << "Applying rotating...";
	ApplyRotatingStereo(wm_.wav, rate);
	cout << "Done" << endl;
}

void ApplyEffectMenu::fade() const
{
	// Enter fade type
	cout << "Fade type: 1 - fade in, 0 - fade out." << endl
		<< "Enter fade type: ";
	const bool is_fade_in = ReadValue<int>([](auto value) {
		return value == 0 || value == 1;
	}) == 1;

	// Enter time
	cout << "Enter fade time in seconds: ";
	const auto fade_time = ReadValue<float>([&](auto value) {
		return value > 0 && value < wm_.wav.GetLengthInSeconds();
	});

	// Enter curve
	cout << "Curve type:" << endl
		<< " 1 - Linear" << endl
		<< " 2 - Logarithmic" << endl
		<< " 3 - Sine" << endl;
	cout << "Enter curve type: ";
	const auto curve_type = static_cast<CurveType>(ReadValue<int>([](auto value) {
		return value >= 1 && value <= 3;
	}));

	// Apply fade
	cout << "Applying fade...";
	if (is_fade_in)
		ApplyFadeIn(wm_.wav, fade_time, curve_type);
	else
		ApplyFadeOut(wm_.wav, fade_time, curve_type);
	cout << "Done" << endl;
}

void ApplyEffectMenu::tremolo() const
{
	cout << "Enter tremolo frequency in Hz: ";
	const auto freq = ReadValue<float>(&GreaterThanZero);

	cout << "Enter dry signal percent (0..1): ";
	const auto dry = ReadValue<float>(&IsNormalizedValue);

	cout << "Applying tremolo...";
	ApplyTremolo(wm_.wav, freq, dry, 1.f - dry);
	cout << "Done" << endl;
}

void ApplyEffectMenu::delay() const
{
	cout << "Enter delay time in ms: ";
	const auto delay_time = ReadValue<float>(&GreaterThanZero);

	cout << "Enter decay: ";
	const auto decay = ReadValue<float>(&IsNormalizedValue);

	cout << "Enter channel num, or 0 for applying on all channels: ";
	const auto channel_idx = ReadValue<size_t>([this](auto value) {
		return value >= 0 && value <= wm_.wav.GetNumChannels();
	}) - 1;

	cout << "Applying delay...";
	if (channel_idx == 0)
		ApplyDelay(wm_.wav, delay_time, decay);
	else
		ApplyDelay(wm_.wav, channel_idx - 1, delay_time, decay);
	cout << "Done" << endl;
}

void ApplyEffectMenu::compressor() const
{
	cout << "Enter threshold (in dB) for compressor: ";
	const auto threshold = ReadValue<float>([](auto value) {
		return value <= 0;
	});

	cout << "Enter ratio:";
	const auto ratio = ReadValue<float>([](auto value) {
		return value >= 1;
	});

	const bool downward = Ask("Compress downward?");

	cout << "Applying compressor...";
	ApplyCompressor(wm_.wav, threshold, ratio, downward);
	cout << "Done" << endl;
}

void ApplyEffectMenu::distortion() const
{
	cout << "Enter distortion drive (0..1): ";
	const auto drive = ReadValue<float>(&IsNormalizedValue);

	cout << "Enter blend (0..1): ";
	const auto blend = ReadValue<float>(&IsNormalizedValue);

	cout << "Applying distortion...";
	ApplyDistortion(wm_.wav, drive, blend);
	cout << "Done" << endl;
}
