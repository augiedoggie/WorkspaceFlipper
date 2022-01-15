// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#include "WorkspaceFlipFilter.h"

#include "Defaults.h"

#include <File.h>
#include <FindDirectory.h>
#include <InterfaceDefs.h>
#include <NodeMonitor.h>
#include <Path.h>
#include <Screen.h>

#include <syslog.h>


WorkspaceFlipFilter::WorkspaceFlipFilter()
	:
	BInputServerFilter(),
	BLooper(),
	fScreenFrame(BScreen().Frame()),
	fMouseJumpMargin(kDefaultMouseJumpMargin),
	fThresholdCounter(0)
{
	_LoadSettings();
	Run();
}


WorkspaceFlipFilter::~WorkspaceFlipFilter()
{
	syslog(LOG_INFO, "WorkspaceFlipFilter::~WorkspaceFlipFilter()");
	stop_watching(this);
}


status_t
WorkspaceFlipFilter::InitCheck()
{
	return B_OK;
}


void
WorkspaceFlipFilter::MessageReceived(BMessage *message)
{
	switch(message->what) {
		case B_NODE_MONITOR:
			_LoadSettings();
			break;
		default:
			BHandler::MessageReceived(message);
	}
}


filter_result
WorkspaceFlipFilter::Filter(BMessage* message, BList* /*outlist*/)
{
	if (message->what != B_MOUSE_MOVED)
		return B_DISPATCH_MESSAGE;

	// don't flip workspaces while the mouse is down
	int32 buttons;
	if (message->FindInt32("buttons", &buttons) == B_OK) {
		if (buttons != 0) {
			fThresholdCounter = 0;
			return B_DISPATCH_MESSAGE;
		}
	}

	BPoint point;
	message->FindPoint("where", &point);

	int32 inactiveHeight = fActiveAreaHeight == 100 ? 0 : (fScreenFrame.Height() - (fScreenFrame.Height()*fActiveAreaHeight/100))/2;
	if (point.y <= fScreenFrame.top + inactiveHeight || point.y >= fScreenFrame.bottom - inactiveHeight) {
		fThresholdCounter = 0;
		return B_DISPATCH_MESSAGE;
	}

	if (point.x <= fScreenFrame.left + fActiveAreaWidth) {
		fThresholdCounter++;

		if (fThresholdCounter < fFlipThreshold)
			return B_DISPATCH_MESSAGE;

		_Flip(-1);
		point.x = fScreenFrame.right - fMouseJumpMargin;
	} else if (point.x >= fScreenFrame.right - fActiveAreaWidth) {
		fThresholdCounter++;

		if (fThresholdCounter < fFlipThreshold)
			return B_DISPATCH_MESSAGE;

		_Flip(1);
		point.x = fScreenFrame.left + fMouseJumpMargin;
	} else {
		// We're not in any of the active areas so reset the counter
		fThresholdCounter = 0;
		return B_DISPATCH_MESSAGE;
	}

	fThresholdCounter = 0;
	message->ReplacePoint("where", point);

	return B_DISPATCH_MESSAGE;
}


void
WorkspaceFlipFilter::_LoadSettings()
{
	syslog(LOG_INFO, "WorkspaceFlipFilter::_LoadSettings()");

	BMessage settingsMessage;

	BPath settingsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
		settingsPath.Append(kSettingsFileName);
		BFile settingsFile;
		if (settingsFile.SetTo(settingsPath.Path(), B_READ_ONLY | B_CREATE_FILE) == B_OK)
			settingsMessage.Unflatten(&settingsFile);

		stop_watching(this);
		node_ref ref;
		settingsFile.GetNodeRef(&ref);
		if (watch_node(&ref, B_WATCH_ALL, NULL, this) != B_OK)
			syslog(LOG_ERR, "WorkspaceFlipFilter: Unable to start node monitoring");
	}

	fFlipThreshold = settingsMessage.GetInt32(kSettingsThresholdKey, kDefaultActivityThreshold);
	fActiveAreaWidth = settingsMessage.GetInt32(kSettingsWidthKey, kDefaultActiveWidth);
	fActiveAreaHeight = settingsMessage.GetInt32(kSettingsHeightKey, kDefaultActiveHeight);

	// ensure fMouseJumpMargin is not in the active area
	fMouseJumpMargin = fActiveAreaWidth + kDefaultMouseJumpMargin;
}


void
WorkspaceFlipFilter::_Flip(int32 workspaceOffset)
{
	int32 workspaceCount = count_workspaces();
	int32 workspace = current_workspace() + workspaceOffset;

	if (workspace < 0)
		workspace = workspaceCount - 1;
	else if (workspace >= workspaceCount)
		workspace = 0;

	activate_workspace(workspace);
}


extern "C"
BInputServerFilter* instantiate_input_filter()
{
	syslog(LOG_INFO, "WorkspaceFlipFilter::instantiate_input_filter()");
	return new (std::nothrow) WorkspaceFlipFilter();
}
