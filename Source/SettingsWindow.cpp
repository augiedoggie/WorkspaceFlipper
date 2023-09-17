// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#include "SettingsWindow.h"
#include "Defaults.h"
#include "ZonePreview.h"

#include <Alert.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <File.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <Path.h>
#include <Slider.h>
#include <String.h>


enum {
	kDefaultsMessage	= 'defs',
	kRevertMessage		= 'rvrt',
	kApplyMessage		= 'aply',
	kActivityMessage	= 'actv',
	kWidthMessage		= 'wdth',
	kHeightMessage		= 'heit'
};

const float kPreviewMinWidth = 300.0;
const float kPreviewMaxWidth = 960.0;
const float kPreviewMaxHeight = 540.0;

const int32 kActivitySliderStep = 5;

const char* kActivityPrefixString = "Activity Threshold: ";
const char* kActivityPostfixString = "px";
const char* kWidthPrefixString = "Active Area Width: ";
const char* kWidthPostfixString = "px";
const char* kHeightPrefixString = "Active Area Height: ";
const char* kHeightPostfixString = "%";


SettingsWindow::SettingsWindow()
	:
	BWindow(BRect(150, 150, 900, 450), "Workspace Flipper Settings", B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE),
	fPreview(new ZonePreview(kDefaultActiveWidth, kDefaultActiveHeight))
{
	fPreview->SetExplicitMinSize(BSize(kPreviewMinWidth, B_SIZE_UNSET));
	fPreview->SetExplicitMaxSize(BSize(kPreviewMaxWidth, kPreviewMaxHeight));

	BBox* previewBBox = new BBox(B_FANCY_BORDER, fPreview);
	previewBBox->SetLabel("Active Area Preview");
	BGroupLayout* layout = new BGroupLayout(B_HORIZONTAL, 0);
	layout->SetInsets(B_USE_SMALL_INSETS, previewBBox->TopBorderOffset() + 10, B_USE_SMALL_INSETS, B_USE_SMALL_INSETS);
	previewBBox->SetLayout(layout);

	fActivitySlider = new BSlider("ActivitySlider", kActivityPrefixString, new BMessage(kActivityMessage), 0, 20, B_HORIZONTAL);
	fActivitySlider->SetLimitLabels("Instant Flip", "Heavy Resistance");
	fActivitySlider->SetModificationMessage(new BMessage(kActivityMessage));

	fWidthSlider = new BSlider("ActiveWidthSlider", kWidthPrefixString, new BMessage(kWidthMessage), 0, 30, B_HORIZONTAL);
	fWidthSlider->SetLimitLabels("0px", "30px");
	fWidthSlider->SetModificationMessage(new BMessage(kWidthMessage));

	fHeightSlider = new BSlider("ActiveHeightSlider", kHeightPrefixString, new BMessage(kHeightMessage), 10, 100, B_HORIZONTAL);
	fHeightSlider->SetLimitLabels("10%", "100%");
	fHeightSlider->SetModificationMessage(new BMessage(kHeightMessage));

	BBox* spacerOne = new BBox(B_PLAIN_BORDER, NULL);
	spacerOne->SetExplicitSize(BSize(B_SIZE_UNSET, 2));

	BBox* spacerTwo = new BBox(B_PLAIN_BORDER, NULL);
	spacerTwo->SetExplicitSize(BSize(B_SIZE_UNSET, 2));

	_LoadSettings();

	// clang-format off
	BLayoutBuilder::Group<>(this, B_HORIZONTAL, 1)
		.Add(previewBBox)
		.AddGroup(B_VERTICAL)
			.Add(fActivitySlider)
			.Add(spacerOne)
			.Add(fWidthSlider)
			.Add(spacerTwo)
			.Add(fHeightSlider)
			.AddGroup(B_HORIZONTAL)
				.Add(new BButton("RevertButton", "Revert", new BMessage(kRevertMessage)))
				.Add(new BButton("DefaultsButton", "Defaults", new BMessage(kDefaultsMessage)))
				.Add(new BButton("ApplyButton", "Apply", new BMessage(kApplyMessage)))
			.End()
			.AddGlue()
			.SetInsets(5)
		.End()
		.SetInsets(5);
	// clang-format on
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kDefaultsMessage:
			_LoadSettings(true);
			break;
		case kRevertMessage:
			_LoadSettings();
			break;
		case kApplyMessage:
			_SaveSettings();
			break;
		case kActivityMessage:
			_UpdateSliderLabel(fActivitySlider, kActivityPrefixString, kActivityPostfixString, kActivitySliderStep);
			break;
		case kWidthMessage:
			_UpdateSliderLabel(fWidthSlider, kWidthPrefixString, kWidthPostfixString);
			fPreview->UpdateDimensions(fWidthSlider->Value(), fHeightSlider->Value());
			break;
		case kHeightMessage:
			_UpdateSliderLabel(fHeightSlider, kHeightPrefixString, kHeightPostfixString);
			fPreview->UpdateDimensions(fWidthSlider->Value(), fHeightSlider->Value());
			break;
		default:
			BWindow::MessageReceived(message);
	}
}


void
SettingsWindow::_LoadSettings(bool useDefaults)
{
	BMessage settingsMessage;

	if (!useDefaults) {
		BPath settingsPath;
		if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
			settingsPath.Append(kSettingsFileName);
			BFile settingsFile;
			if (settingsFile.SetTo(settingsPath.Path(), B_READ_ONLY) == B_OK)
				settingsMessage.Unflatten(&settingsFile);
		} else
			(new BAlert("Settings Error", "Error locating settings directory", "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_WARNING_ALERT))->Go();
	}

	fActivitySlider->SetValue(settingsMessage.GetInt32(kSettingsThresholdKey, kDefaultActivityThreshold) / kActivitySliderStep);
	fWidthSlider->SetValue(settingsMessage.GetInt32(kSettingsWidthKey, kDefaultActiveWidth));
	fHeightSlider->SetValue(settingsMessage.GetInt32(kSettingsHeightKey, kDefaultActiveHeight));

	_UpdateSliderLabel(fActivitySlider, kActivityPrefixString, kActivityPostfixString, kActivitySliderStep);
	_UpdateSliderLabel(fWidthSlider, kWidthPrefixString, kWidthPostfixString);
	_UpdateSliderLabel(fHeightSlider, kHeightPrefixString, kHeightPostfixString);

	fPreview->UpdateDimensions(fWidthSlider->Value(), fHeightSlider->Value());
}


void
SettingsWindow::_SaveSettings()
{
	BPath settingsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) != B_OK) {
		(new BAlert("Settings Error", "Error locating settings directory", "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_WARNING_ALERT))->Go();
		return;
	}

	settingsPath.Append(kSettingsFileName);
	BFile settingsFile;
	if (settingsFile.SetTo(settingsPath.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE) != B_OK) {
		(new BAlert("Settings Error", "Error reading settings file", "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_WARNING_ALERT))->Go();
		return;
	}

	BMessage settingsMessage;
	settingsMessage.AddInt32(kSettingsThresholdKey, fActivitySlider->Value() * kActivitySliderStep);
	settingsMessage.AddInt32(kSettingsWidthKey, fWidthSlider->Value());
	settingsMessage.AddInt32(kSettingsHeightKey, fHeightSlider->Value());
	if (settingsMessage.Flatten(&settingsFile) != B_OK)
		(new BAlert("Settings Error", "Error saving settings", "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_WARNING_ALERT))->Go();
}


void
SettingsWindow::_UpdateSliderLabel(BSlider* slider, const char* prefix, const char* postfix, int32 step)
{
	BString label(prefix);
	label << slider->Value() * step << postfix;
	slider->SetLabel(label);
}
