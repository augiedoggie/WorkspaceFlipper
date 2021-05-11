// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#include "ZonePreview.h"

#include <Bitmap.h>
#include <Screen.h>


ZonePreview::ZonePreview(int32 activeWidth, int32 activeHeight)
	:
	BView("FlipperPreview", B_WILL_DRAW | B_FRAME_EVENTS | B_SUBPIXEL_PRECISE | B_FULL_UPDATE_ON_RESIZE),
	fScreenBitmap(NULL)
{
	SetViewColor(B_TRANSPARENT_COLOR);

	if (!BScreen().IsValid())
		return;

	fScreenFrame = BScreen().Frame();

	if (BScreen().GetBitmap(&fScreenBitmap) != B_OK) {
		fScreenBitmap = new BBitmap(fScreenFrame, B_RGB32);
		//TODO use generic screen image here
	}

	UpdateDimensions(activeWidth, activeHeight);
}


ZonePreview::~ZonePreview()
{
	delete fScreenBitmap;
}


void
ZonePreview::Draw(BRect updateRect)
{
	float aspectRatio = fScreenFrame.Width()/fScreenFrame.Height();
	BRect viewRect(0, 0, Bounds().Width(), Bounds().Width()/aspectRatio);
	if (viewRect.Height() > Bounds().Height())
		viewRect.Set(0, 0, Bounds().Height() * aspectRatio, Bounds().Height());

	SetHighUIColor(B_PANEL_BACKGROUND_COLOR);
	FillRect(updateRect);

	if (fScreenBitmap != NULL)
		DrawBitmap(fScreenBitmap, viewRect);

	SetHighColor(255, 0, 0);
	float scale = viewRect.Width()/fScreenFrame.Width();

	//TODO BRegion::ScaleBy() seems to be broken
	//BRegion scaleRegion(fActiveRegion);
	//scaleRegion.ScaleBy(scale, scale);
	//FillRegion(&scaleRegion);
	//fActiveRegion.PrintToStream();

	BRect one(fActiveRegion.RectAt(0));
	FillRect(BRect(one.left*scale, one.top*scale, (one.right*scale), one.bottom*scale));
	one = BRect(fActiveRegion.RectAt(1));
	FillRect(BRect(one.left*scale, one.top*scale, one.right*scale, one.bottom*scale));

	BView::Draw(updateRect);
}


#ifdef PREVIEW_HEIGHT_FOR_WIDTH
void
ZonePreview::GetHeightForWidth(float width, float* min, float* max, float* preferred)
{
	float aspectRatio = fScreenFrame.Width() / fScreenFrame.Height();
	float calcHeight = floor(width / aspectRatio);

	if (min != NULL) *min = calcHeight;
	if (max != NULL) *max = calcHeight;
	if (preferred != NULL) *preferred = calcHeight;
}


bool
ZonePreview::HasHeightForWidth()
{
	return true;
}
#endif


void
ZonePreview::UpdateDimensions(int32 activeWidth, int32 activeHeight)
{
	float safeHeight = (fScreenFrame.Height()-(fScreenFrame.Height()*activeHeight/100))/2;
	fActiveRegion.MakeEmpty();
	fActiveRegion.Include(BRect(0, safeHeight, activeWidth, fScreenFrame.bottom-safeHeight));
	fActiveRegion.Include(BRect(fScreenFrame.right-activeWidth, safeHeight, fScreenFrame.right, fScreenFrame.bottom-safeHeight));
	Invalidate();
}
