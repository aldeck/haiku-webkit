/*
 * Copyright (C) 2007 Andrea Anzani <andrea.anzani@gmail.com>
 * Copyright (C) 2007 Ryan Leavengood <leavengood@gmail.com>
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
#include "WebWindow.h"

#include "WebSettings.h"
#include "WebView.h"
#include "WebViewConstants.h"

#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <StringView.h>
#include <TextControl.h>

#include <stdio.h>

enum {
    ICON_FOR_URL_RECEIVED = 'icfu'
};

BWebWindow::BWebWindow(BRect frame, const char* title, window_look look,
        window_feel feel, uint32 flags, uint32 workspace)
    : BWindow(frame, title, look, feel, flags, workspace)
    , fWebView(0)
{
    SetLayout(new BGroupLayout(B_HORIZONTAL));

    // NOTE: Do NOT change these because you think Redo should be on Cmd-Y!
    AddShortcut('Z', B_COMMAND_KEY, new BMessage(B_UNDO));
    AddShortcut('Y', B_COMMAND_KEY, new BMessage(B_UNDO));
    AddShortcut('Z', B_COMMAND_KEY | B_SHIFT_KEY, new BMessage(B_REDO));
    AddShortcut('Y', B_COMMAND_KEY | B_SHIFT_KEY, new BMessage(B_REDO));
}

BWebWindow::~BWebWindow()
{
}

void BWebWindow::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case LOAD_ONLOAD_HANDLE:
        // NOTE: Supposedly this notification is sent when the so-called
        // "onload" event has been passed and executed to scripts in the
        // site.
        break;
    case LOAD_DL_COMPLETED:
        // NOTE: All loading has finished. We currently handle this via
        // the progress notification. But it doesn't hurt to call the hook
        // one more time.
        LoadProgress(100, _WebViewForMessage(message));
        break;
    case LOAD_DOC_COMPLETED:
        // NOTE: This events means the DOM document is ready.
        break;
    case JAVASCRIPT_WINDOW_OBJECT_CLEARED:
        // NOTE: No idea what this event actually means.
        break;
    case NAVIGATION_REQUESTED: {
        BString url;
        if (message->FindString("url", &url) == B_OK) {
            NavigationRequested(url, _WebViewForMessage(message));
            _FetchIconForURL(url, *message);
        }
        break;
    }
    case UPDATE_HISTORY: {
        BString url;
        if (message->FindString("url", &url) == B_OK)
            UpdateGlobalHistory(url);
        break;
    }
    case NEW_WINDOW_REQUESTED: {
        bool primaryAction = false;
        message->FindBool("primary", &primaryAction);
        BString url;
        if (message->FindString("url", &url) == B_OK)
            NewWindowRequested(url, primaryAction);
        break;
    }
    case NEW_PAGE_CREATED: {
        // The FrameLoaderClient blocks until we have processed the message
        // and sent a default reply. That's why the pointer is guaranteed
        // to be still valid.
        BWebView* view;
        if (message->FindPointer("view", reinterpret_cast<void**>(&view)) != B_OK)
        	break;
        BRect windowFrame;
        if (message->FindRect("frame", &windowFrame) != B_OK)
        	windowFrame = BRect();
        bool modalDialog;
        if (message->FindBool("modal", &modalDialog) != B_OK)
        	modalDialog = false;
        bool resizable;
        if (message->FindBool("resizable", &resizable) != B_OK)
        	resizable = true;

        NewPageCreated(view, windowFrame, modalDialog, resizable);
        break;
    }
    case CLOSE_WINDOW_REQUESTED:
        CloseWindowRequested(_WebViewForMessage(message));
        break;
    case LOAD_NEGOTIATING: {
        BString url;
        if (message->FindString("url", &url) == B_OK) {
            LoadNegotiating(url, _WebViewForMessage(message));
            _FetchIconForURL(url, *message);
        }
        break;
    }
    case LOAD_COMMITTED: {
        BString url;
        if (message->FindString("url", &url) == B_OK)
            LoadCommitted(url, _WebViewForMessage(message));
        break;
    }
    case LOAD_PROGRESS: {
        float progress;
        if (message->FindFloat("progress", &progress) == B_OK)
            LoadProgress(progress, _WebViewForMessage(message));
        break;
    }
    case LOAD_FAILED: {
        BString url;
        if (message->FindString("url", &url) == B_OK)
            LoadFailed(url, _WebViewForMessage(message));
        break;
    }
    case LOAD_FINISHED: {
        BString url;
        if (message->FindString("url", &url) == B_OK)
            LoadFinished(url, _WebViewForMessage(message));
        break;
    }
    case TITLE_CHANGED: {
        BString title;
        if (message->FindString("title", &title) == B_OK)
            TitleChanged(title, _WebViewForMessage(message));
        break;
    }
    case ICON_RECEIVED: {
    	// The icon is now in the database.
        BString url;
        if (message->FindString("url", &url) == B_OK)
            _FetchIconForURL(url, *message);
        break;
    }
    case ICON_FOR_URL_RECEIVED: {
        BMessage iconArchive;
        if (message->FindMessage("icon", &iconArchive) == B_OK) {
        	BBitmap icon(&iconArchive);
            IconReceived(&icon, _WebViewForMessage(message));
        } else
            IconReceived(NULL, _WebViewForMessage(message));
    	break;
    }
    case RESIZING_REQUESTED: {
        BRect rect;
        if (message->FindRect("rect", &rect) == B_OK)
            ResizeRequested(rect.Width(), rect.Height(), _WebViewForMessage(message));
        break;
    }
    case SET_STATUS_TEXT: {
        BString text;
        if (message->FindString("text", &text) == B_OK)
            StatusChanged(text, _WebViewForMessage(message));
        break;
    }
    case UPDATE_NAVIGATION_INTERFACE: {
        bool canGoBackward = false;
        bool canGoForward = false;
        bool canStop = false;
        message->FindBool("can go backward", &canGoBackward);
        message->FindBool("can go forward", &canGoForward);
        message->FindBool("can stop", &canStop);
        NavigationCapabilitiesChanged(canGoBackward, canGoForward, canStop, _WebViewForMessage(message));
        break;
    }
    case AUTHENTICATION_CHALLENGE: {
	    BString text;
	    bool rememberCredentials = false;
	    uint32 failureCount = 0;
	    BString user;
	    BString password;

	    message->FindString("text", &text);
	    message->FindString("user", &user);
	    message->FindString("password", &password);
	    message->FindUInt32("failureCount", &failureCount);

        if (!AuthenticationChallenge(text, user, password, rememberCredentials,
        		failureCount, _WebViewForMessage(message))) {
        	message->SendReply((uint32)0);
        	break;
        }

	    BMessage reply;
	    reply.AddString("user", user);
	    reply.AddString("password", password);
	    reply.AddBool("rememberCredentials", rememberCredentials);
	    message->SendReply(&reply);
        break;
    }

    case TOOLBARS_VISIBILITY: {
        bool flag;
        if (message->FindBool("flag", &flag) == B_OK)
            SetToolBarsVisible(flag, _WebViewForMessage(message));
        break;
    }
    case STATUSBAR_VISIBILITY: {
        bool flag;
        if (message->FindBool("flag", &flag) == B_OK)
            SetStatusBarVisible(flag, _WebViewForMessage(message));
        break;
    }
    case MENUBAR_VISIBILITY: {
        bool flag;
        if (message->FindBool("flag", &flag) == B_OK)
            SetMenuBarVisible(flag, _WebViewForMessage(message));
        break;
    }
    case SET_RESIZABLE: {
        bool flag;
        if (message->FindBool("flag", &flag) == B_OK)
            SetResizable(flag, _WebViewForMessage(message));
        break;
    }

    case B_MOUSE_WHEEL_CHANGED:
    	if (fWebView)
        	fWebView->MessageReceived(message);
        break;

    default:
        BWindow::MessageReceived(message);
        break;
    }
}

bool BWebWindow::QuitRequested()
{
    // Do this here, so WebKit tear down happens earlier.
    if (fWebView)
        fWebView->Shutdown();

    return true;
}

// #pragma mark -

void BWebWindow::SetCurrentWebView(BWebView* view)
{
	fWebView = view;
}

BWebView* BWebWindow::CurrentWebView() const
{
	return fWebView;
}

// #pragma mark - Notification API

void BWebWindow::NavigationRequested(const BString& url, BWebView* view)
{
}

void BWebWindow::NewWindowRequested(const BString& url, bool primaryAction)
{
}

void BWebWindow::NewPageCreated(BWebView* view, BRect windowFrame,
    bool modalDialog, bool resizable)
{
	if (!windowFrame.IsValid())
		windowFrame = Frame().OffsetByCopy(10, 10);

	uint32 flags = Flags();

	window_look look;
	window_feel feel;
	if (modalDialog) {
		feel = B_MODAL_APP_WINDOW_FEEL;
		look = B_BORDERED_WINDOW_LOOK;
	} else {
	    look = B_TITLED_WINDOW_LOOK;
	    feel = B_NORMAL_WINDOW_FEEL;
	}
	if (!resizable)
		flags |= B_NOT_RESIZABLE;

    BWebWindow* window = new BWebWindow(windowFrame, "WebKit window",
        look, feel, flags);

    window->AddChild(view);
    window->SetCurrentWebView(view);

    window->Show();
}

void BWebWindow::CloseWindowRequested(BWebView* view)
{
    PostMessage(B_QUIT_REQUESTED);
}

void BWebWindow::LoadNegotiating(const BString& url, BWebView* view)
{
}

void BWebWindow::LoadCommitted(const BString& url, BWebView* view)
{
}

void BWebWindow::LoadProgress(float progress, BWebView* view)
{
}

void BWebWindow::LoadFailed(const BString& url, BWebView* view)
{
}

void BWebWindow::LoadFinished(const BString& url, BWebView* view)
{
}

void BWebWindow::TitleChanged(const BString& title, BWebView* view)
{
    SetTitle(title.String());
}

void BWebWindow::IconReceived(const BBitmap* icon, BWebView* view)
{
}

void BWebWindow::ResizeRequested(float width, float height, BWebView* view)
{
    ResizeTo(width, height);
}

void BWebWindow::SetToolBarsVisible(bool flag, BWebView* view)
{
}

void BWebWindow::SetStatusBarVisible(bool flag, BWebView* view)
{
}

void BWebWindow::SetMenuBarVisible(bool flag, BWebView* view)
{
}

void BWebWindow::SetResizable(bool flag, BWebView* view)
{
    if (flag)
        SetFlags(Flags() & ~B_NOT_RESIZABLE);
    else
        SetFlags(Flags() | B_NOT_RESIZABLE);
}

void BWebWindow::StatusChanged(const BString& statusText, BWebView* view)
{
}

void BWebWindow::NavigationCapabilitiesChanged(bool canGoBackward,
    bool canGoForward, bool canStop, BWebView* view)
{
}

void BWebWindow::UpdateGlobalHistory(const BString& url)
{
}

bool BWebWindow::AuthenticationChallenge(BString message, BString& inOutUser,
	BString& inOutPassword, bool& inOutRememberCredentials, uint32 failureCount,
	BWebView* view)
{
	return false;
}

// #pragma mark - private

void BWebWindow::_FetchIconForURL(const BString& url, const BMessage& message)
{
	BMessage reply(message);
	reply.what = ICON_FOR_URL_RECEIVED;
	BMessenger target(this);
	BWebSettings::SendIconForURL(url, reply, target);
}

BWebView* BWebWindow::_WebViewForMessage(const BMessage* message) const
{
	// Default to the current BWebView, if there is none in the message.
	BWebView* view = fWebView;
	message->FindPointer("view", reinterpret_cast<void**>(&view));
	return view;
}

