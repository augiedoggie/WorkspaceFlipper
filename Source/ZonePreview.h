// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#ifndef _ZONEPREVIEW_H_
#define _ZONEPREVIEW_H_

#include <Region.h>
#include <View.h>


class BBitmap;


class ZonePreview : public BView {

public:
						ZonePreview(int32 activeWidth, int32 activeHeight);
						~ZonePreview();

	virtual	void		Draw(BRect updateRect);
#ifdef PREVIEW_HEIGHT_FOR_WIDTH
	virtual	void		GetHeightForWidth(float width, float* min, float* max, float* preferred);
	virtual	bool		HasHeightForWidth();
#endif

			void		UpdateDimensions(int32 activeWidth, int32 activeHeight);

private:
			BBitmap*	fScreenBitmap;
			BRegion		fActiveRegion;
			BRect		fScreenFrame;
};


#endif // _ZONEPREVIEW_H_
