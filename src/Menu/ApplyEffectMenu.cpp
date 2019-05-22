#include <iostream>
#include "ApplyEffectMenu.h"
#include "WavManager.h"
#include "../Effects.h"

using namespace std;
using namespace effects;

ApplyEffectMenu::ApplyEffectMenu(std::stack<std::unique_ptr<Menu>>* menuStack): Menu(menuStack)
{
	m_menuItems = {
		"Back",
		"Mono -> Stereo",
		"Reverse",
		"Volume",
		"Reverberation",
		"Rotating"
		//TODO: Add more effects
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

		default: throw out_of_range("Effect idx out-of-range: " + std::to_string(selectedIndex));
	}

	m_wm.isFileUnsaved = true;
	pressAnyKey();
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
	cout << "Enter volume for increasing in dB:";
	float volume = 0;
	cin >> volume;

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
		cout << "File must be in stereo. Convert into it?" << endl;
		if (!dialog())
		{
			cout << "Aborting." << endl;
			return;
		}

		monoToStereo();
	}

	cout << "Enter rotating rate in seconds: ";
	float rate = 0;
	cin >> rate;

	cout << "Applying rotating...";
	applyRotatingStereo(m_wm.wav, rate);
	cout << "Done" << endl;
}
