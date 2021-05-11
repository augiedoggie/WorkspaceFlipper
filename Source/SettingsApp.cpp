// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#include "SettingsWindow.h"

#include <Application.h>
#include <Alert.h>


class SettingsApp : public BApplication {
public:
	SettingsApp()
		:
		BApplication("application/x-vnd.cpr.WorkspaceFlipperSettings")
	{
		SettingsWindow* window = new SettingsWindow();
		window->Lock();
		window->Show();
		window->Unlock();
	}

	void
	AboutRequested()
	{
		(new BAlert("AboutWindow", "Workspace Flipper\nWritten by Chris Roberts", "OK", NULL, NULL, B_WIDTH_FROM_LABEL))->Go();
	}
};


int
main(int /*argc*/, char** /*argv*/)
{
	SettingsApp app;
	app.Run();

	return 0;
}
