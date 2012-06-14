/*
 * Copyright (C) 2009 Maxime Simon <simon.maxime@gmail.com>
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebView.h"

#include "NotImplemented.h"
#include "WebPage.h"
#include "WebViewConstants.h"
#include <Alert.h>
#include <AppDefs.h>
#include <Application.h>
#include <Bitmap.h>
#include <Region.h>
#include <Window.h>
#include <stdio.h>
#include <stdlib.h>

using namespace WebCore;

BWebView::UserData::~UserData()
{
}

BWebView::BWebView(const char* name)
    : BView(name, B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE)
    , fLastMouseButtons(0)
    , fOffscreenBitmap(0)
    , fOffscreenViewClean(false)
    , fWebPage(new BWebPage(this))
    , fUserData(0)
{
    fWebPage->Init();
    // TODO: Should add this to the "current" looper, but that looper needs to
    // stay around regardless of windows opening/closing. Adding it to the
    // app looper is the safest bet for now.
    if (be_app->Lock()) {
        be_app->AddHandler(fWebPage);
        be_app->Unlock();
    }

    FrameResized(Bounds().Width(), Bounds().Height());

    SetViewColor(B_TRANSPARENT_COLOR);
}

BWebView::~BWebView()
{
	if (fOffscreenBitmap) {
		fOffscreenBitmap->Lock();
		delete fOffscreenBitmap;
	}
	SetUserData(0);
}

void BWebView::Shutdown()
{
	if (Window())
		RemoveSelf();
    fWebPage->Shutdown();
}

// #pragma mark - BView hooks

void BWebView::AttachedToWindow()
{
    fWebPage->SetListener(BMessenger(Window()));
    fWebPage->activated(Window()->IsActive());
}

void BWebView::DetachedFromWindow()
{
}

void BWebView::Show()
{
    BView::Show();
    fWebPage->setVisible(true);
}

void BWebView::Hide()
{
	fWebPage->setVisible(false);
    BView::Hide();
}

void BWebView::Draw(BRect rect)
{
#if 0
    if (!fOffscreenViewClean) {
        fWebPage->draw(rect);
        return;
    }

    if (!fOffscreenBitmap->Lock()) {
        SetHighColor(255, 0, 0);
        FillRect(rect);
        return;
    }
#else
    if (!fOffscreenViewClean || !fOffscreenBitmap->Lock()) {
        SetHighColor(255, 255, 255);
        FillRect(rect);
        return;
    }
#endif

    fOffscreenView->Sync();
    DrawBitmap(fOffscreenBitmap, fOffscreenView->Bounds(),
        fOffscreenView->Bounds());

    fOffscreenBitmap->Unlock();
}

void BWebView::FrameResized(float width, float height)
{
    _ResizeOffscreenView(width + 1, height + 1);
    fWebPage->frameResized(width, height);
}

void BWebView::GetPreferredSize(float* width, float* height)
{
	// This needs to be implemented, since the default BView implementation
	// is to return the current width/height of the view. The default
	// implementations for Min/Max/PreferredSize() will then work for us.
	if (width)
		*width = 100;
	if (height)
		*height = 100;
}

void BWebView::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case B_MOUSE_WHEEL_CHANGED: {
        BPoint where;
        uint32 buttons;
        GetMouse(&where, &buttons);
        BPoint screenWhere = ConvertToScreen(where);
        fWebPage->mouseWheelChanged(message, where, screenWhere);
//        // NOTE: This solves a bug in WebKit itself, it should issue a mouse
//        // event when scrolling by wheel event. The effects of the this bug
//        // can be witnessed in Safari as well.
//        BMessage mouseMessage(B_MOUSE_MOVED);
//        mouseMessage.AddPoint("be:view_where", where);
//        mouseMessage.AddPoint("screen_where", screenWhere);
//        mouseMessage.AddInt32("buttons", buttons);
//        mouseMessage.AddInt32("modifiers", modifiers());
//        fWebPage->mouseEvent(&mouseMessage, where, screenWhere);
        break;
    }

    case B_SELECT_ALL:
    case B_COPY:
    case B_CUT:
    case B_PASTE:
    case B_UNDO:
    case B_REDO:
        fWebPage->standardShortcut(message);
        break;

    case B_FIND_STRING_RESULT:
        Window()->PostMessage(message);
        break;

    default:
        BView::MessageReceived(message);
        break;
    }
}

void BWebView::MakeFocus(bool focused)
{
	BView::MakeFocus(focused);
	fWebPage->focused(focused);
}

void BWebView::WindowActivated(bool activated)
{
    fWebPage->activated(activated);
}

void BWebView::MouseMoved(BPoint where, uint32, const BMessage*)
{
    _DispatchMouseEvent(where, B_MOUSE_MOVED);
}

void BWebView::MouseDown(BPoint where)
{
	MakeFocus(true);
    SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
    _DispatchMouseEvent(where, B_MOUSE_DOWN);
}

void BWebView::MouseUp(BPoint where)
{
    _DispatchMouseEvent(where, B_MOUSE_UP);
}

void BWebView::KeyDown(const char*, int32)
{
    _DispatchKeyEvent(B_KEY_DOWN);
}

void BWebView::KeyUp(const char*, int32)
{
    _DispatchKeyEvent(B_KEY_UP);
}

// #pragma mark - public API

BString BWebView::MainFrameTitle() const
{
    return fWebPage->MainFrameTitle();
}

BString BWebView::MainFrameRequestedURL() const
{
    return fWebPage->MainFrameRequestedURL();
}

BString BWebView::MainFrameURL() const
{
    return fWebPage->MainFrameURL();
}

void BWebView::LoadURL(const char* urlString, bool aquireFocus)
{
    fWebPage->LoadURL(urlString);
	// Always focus the web view after firing off a load request.
	// This behavior is also observed in Firefox.
	if (LockLooper()) {
		if (aquireFocus && !IsFocus())
			MakeFocus(true);
		UnlockLooper();
	}
}

void BWebView::Reload()
{
    fWebPage->Reload();
}

void BWebView::GoBack()
{
    fWebPage->GoBack();
}

void BWebView::GoForward()
{
    fWebPage->GoForward();
}

void BWebView::StopLoading()
{
    fWebPage->StopLoading();
}

void BWebView::IncreaseZoomFactor(bool textOnly)
{
	fWebPage->ChangeZoomFactor(1, textOnly);
}

void BWebView::DecreaseZoomFactor(bool textOnly)
{
	fWebPage->ChangeZoomFactor(-1, textOnly);
}

void BWebView::ResetZoomFactor()
{
	fWebPage->ChangeZoomFactor(0, false);
}

void BWebView::FindString(const char* string, bool forward ,
    bool caseSensitive, bool wrapSelection, bool startInSelection)
{
	fWebPage->FindString(string, forward, caseSensitive,
	    wrapSelection, startInSelection);
}

void BWebView::SendFakeMouseMovedEvent()
{
	if (!LockLooper())
	    return;

    BPoint where;
    uint32 buttons;
    GetMouse(&where, &buttons);
    BPoint screenWhere = ConvertToScreen(where);
    _DispatchFakeMouseMovedEvent(where, screenWhere, buttons);
    UnlockLooper();
}

void BWebView::SetUserData(BWebView::UserData* userData)
{
	if (fUserData == userData)
		return;

	delete fUserData;
	fUserData = userData;
}

BWebView::UserData* BWebView::GetUserData() const
{
	return fUserData;
}

// #pragma mark - API for WebPage only

void BWebView::SetOffscreenViewClean(BRect cleanRect, bool immediate)
{
    if (LockLooper()) {
        fOffscreenViewClean = true;
        if (immediate)
            Draw(cleanRect);
        else
            Invalidate(cleanRect);
        UnlockLooper();
    }
}

void BWebView::InvalidateOffscreenView()
{
    if (LockLooper()) {
        fOffscreenViewClean = false;
        Invalidate();
        UnlockLooper();
    }
}

// #pragma mark - private

void BWebView::_ResizeOffscreenView(int width, int height)
{
    BRect bounds(0, 0, width - 1, height - 1);
    if (fOffscreenBitmap) {
        fOffscreenBitmap->Lock();
        if (fOffscreenBitmap->Bounds().Contains(bounds)) {
            // Just resize the view and clear the exposed parts, but never
            // shrink the bitmap).
            BRect oldViewBounds(fOffscreenView->Bounds());
            fOffscreenView->ResizeTo(width - 1, height - 1);
            BRegion exposed(fOffscreenView->Bounds());
            exposed.Exclude(oldViewBounds);
            fOffscreenView->FillRegion(&exposed, B_SOLID_LOW);
            fOffscreenBitmap->Unlock();
            return;
        }
    }
    BBitmap* oldBitmap = fOffscreenBitmap;
    BView* oldView = fOffscreenView;

    fOffscreenBitmap = new BBitmap(bounds, B_RGB32, true);
    if (fOffscreenBitmap->InitCheck() != B_OK) {
        BAlert* alert = new BAlert("Internal error", "Unable to create off-screen bitmap for WebKit contents.", "OK");
        alert->Go();
        exit(1);
    }
    fOffscreenView = new BView(bounds, "WebKit offscreen view", 0, 0);
    fOffscreenBitmap->Lock();
    fOffscreenBitmap->AddChild(fOffscreenView);

    if (oldBitmap) {
        // Transfer the old bitmap contents (just the visible part) and
        // clear the rest.
        BRegion region(fOffscreenView->Bounds());
        BRect oldViewBounds = oldView->Bounds();
        region.Exclude(oldViewBounds);
        fOffscreenView->DrawBitmap(oldBitmap, oldViewBounds, oldViewBounds);
        fOffscreenView->FillRegion(&region, B_SOLID_LOW);
        delete oldBitmap;
            // Takes care of old fOffscreenView too.
    }

    fOffscreenBitmap->Unlock();
}

void BWebView::_DispatchMouseEvent(const BPoint& where, uint32 sanityWhat)
{
    BMessage* message = Looper()->CurrentMessage();
    if (!message || message->what != sanityWhat)
        return;

	if (sanityWhat == B_MOUSE_UP) {
		// the activation click may contain no previous buttons
		if (!fLastMouseButtons)
			return;

		message->AddInt32("previous buttons", fLastMouseButtons);
	} else
		message->FindInt32("buttons", (int32*)&fLastMouseButtons);

    fWebPage->mouseEvent(message, where, ConvertToScreen(where));
}

void BWebView::_DispatchFakeMouseMovedEvent(const BPoint& where,
	const BPoint& screenWhere, uint32 buttons)
{
    // NOTE: This solves a bug in WebKit itself, it should issue a mouse
    // event when scrolling by wheel event. The effects of the this bug
    // can be witnessed in Safari as well.
    BMessage mouseMessage(B_MOUSE_MOVED);
    mouseMessage.AddPoint("be:view_where", where);
    mouseMessage.AddPoint("screen_where", screenWhere);
    mouseMessage.AddInt32("buttons", buttons);
    mouseMessage.AddInt32("modifiers", modifiers());
    fWebPage->mouseEvent(&mouseMessage, where, screenWhere);
}

void BWebView::_DispatchKeyEvent(uint32 sanityWhat)
{
    BMessage* message = Looper()->CurrentMessage();
    if (!message || message->what != sanityWhat)
        return;

    fWebPage->keyEvent(message);
}

