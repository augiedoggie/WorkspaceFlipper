// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#ifndef _SETTINGSWINDOW_H_
#define _SETTINGSWINDOW_H_

#include <Window.h>


class ZonePreview;

class BSlider;


class SettingsWindow : public BWindow {

public:
						SettingsWindow();

	virtual	void		MessageReceived(BMessage* message);

private:
			void		_UpdateSliderLabel(BSlider* slider, const char* prefix, const char* postfix, int32 step = 1);
			void		_LoadSettings(bool useDefaults = false);
			void		_SaveSettings();

			ZonePreview*
						fPreview;
			BSlider*	fActivitySlider;
			BSlider*	fWidthSlider;
			BSlider*	fHeightSlider;
};

#endif // _SETTINGSWINDOW_H_
