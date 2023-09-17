// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#ifndef _WORKSPACEFLIPFILTER_H_
#define _WORKSPACEFLIPFILTER_H_

#include <Handler.h>
#include <InputServerFilter.h>


class WorkspaceFlipFilter : public BInputServerFilter, BHandler {

public:
							WorkspaceFlipFilter();
							~WorkspaceFlipFilter();

	virtual	status_t		InitCheck();
	virtual	filter_result	Filter(BMessage* message, BList* outlist);
	virtual	void			MessageReceived(BMessage* message);

private:
			void			_Flip(int32 workspaceOffset);
			void			_LoadSettings();

			BLooper*		fNodeWatchLooper;
			BRect			fScreenFrame;
			float			fActiveAreaHeight;
			float			fActiveAreaWidth;
			float			fMouseJumpMargin;
			int32			fFlipThreshold;
			int32			fThresholdCounter;
};

#endif // _WORKSPACEFLIPFILTER_H_
