/*
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

#ifndef LauncherWindow_h
#define LauncherWindow_h

#include "WebWindow.h"
#include <Messenger.h>
#include <String.h>

class BButton;
class BCheckBox;
class BLayoutItem;
class BMenu;
class BStatusBar;
class BStringView;
class BTextControl;
class IconButton;
class WebTabView;
class BWebView;

enum ToolbarPolicy {
    HaveToolbar,
    DoNotHaveToolbar
};

enum {
    NEW_WINDOW = 'nwnd',
    NEW_TAB = 'ntab',
    WINDOW_OPENED = 'wndo',
    WINDOW_CLOSED = 'wndc',
    SHOW_DOWNLOAD_WINDOW = 'sdwd'
};

class LauncherWindow : public BWebWindow {
public:
    LauncherWindow(BRect frame, const BMessenger& downloadListener,
        ToolbarPolicy = HaveToolbar);
    virtual ~LauncherWindow();

    virtual void MessageReceived(BMessage* message);
    virtual bool QuitRequested();
    virtual void MenusBeginning();

    void newTab(const BString& url, bool select);

private:
    // WebPage notification API implementations
    virtual void NavigationRequested(const BString& url, BWebView* view);
    virtual void NewWindowRequested(const BString& url, bool primaryAction);
    virtual void LoadNegotiating(const BString& url, BWebView* view);
    virtual void LoadCommited(const BString& url, BWebView* view);
    virtual void LoadProgress(float progress, BWebView* view);
    virtual void LoadFailed(const BString& url, BWebView* view);
    virtual void LoadFinished(const BString& url, BWebView* view);
    virtual void TitleChanged(const BString& title, BWebView* view);
    virtual void ResizeRequested(float width, float height, BWebView* view);
    virtual void SetToolBarsVisible(bool flag, BWebView* view);
    virtual void SetStatusBarVisible(bool flag, BWebView* view);
    virtual void SetMenuBarVisible(bool flag, BWebView* view);
    virtual void SetResizable(bool flag, BWebView* view);
    virtual void StatusChanged(const BString& status, BWebView* view);
    virtual void NavigationCapabilitiesChanged(bool canGoBackward,
        bool canGoForward, bool canStop, BWebView* view);
    virtual void UpdateGlobalHistory(const BString& url);
    virtual void AuthenticationChallenge(BMessage* challenge);

    void updateTitle(const BString &title);

private:
    BMessenger m_downloadListener;
    BMenuBar* m_menuBar;
    BMenu* m_goMenu;
    IconButton* m_BackButton;
    IconButton* m_ForwardButton;
    IconButton* m_StopButton;
    BTextControl* m_url;
    BString m_loadedURL;
    BStringView* m_statusText;
    BStatusBar* m_loadingProgressBar;
    BLayoutItem* m_findGroup;
    BTextControl* m_findTextControl;
    BCheckBox* m_findCaseSensitiveCheckBox;
    WebTabView* m_tabView;
};

#endif // LauncherWindow_h

