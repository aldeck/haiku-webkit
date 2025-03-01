/*
 * Copyright (C) 2006 Don Gibson <dgibson77@gmail.com>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Trolltech ASA
 * Copyright (C) 2007 Ryan Leavengood <leavengood@gmail.com> All rights reserved.
 * Copyright (C) 2009 Maxime Simon <simon.maxime@gmail.com> All rights reserved.
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 * Copyright (C) 2010 Michael Lotz <mmlr@mlotz.ch>
 *
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
#include "FrameLoaderClientHaiku.h"

#include "AuthenticationChallenge.h"
#include "Credential.h"
#include "CachedFrame.h"
#include "DocumentLoader.h"
#include "FormState.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameNetworkingContext.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "HTMLFormElement.h"
#include "HTMLFrameOwnerElement.h"
#include "HTTPParsers.h"
#include "HTTPStatusCodes.h"
#include "IconDatabase.h"
#include "MouseEvent.h"
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformString.h"
#include "PluginData.h"
#include "PluginDatabase.h"
#include "PluginView.h"
#include "ProgressTracker.h"
#include "RenderFrame.h"
#include "ResourceRequest.h"
#include "SchemeRegistry.h"
#include "ScriptController.h"
#include "Settings.h"
#include "WebFrame.h"
#include "WebFramePrivate.h"
#include "WebKitInfo.h"
#include "WebPage.h"
#include "WebView.h"
#include "WebViewConstants.h"

#include <Alert.h>
#include <Bitmap.h>
#include <Entry.h>
#include <Locale.h>
#include <Message.h>
#include <MimeType.h>
#include <String.h>
#include <debugger.h>

//#define TRACE_FRAME_LOADER_CLIENT
#ifdef TRACE_FRAME_LOADER_CLIENT
#   define CLASS_NAME "FLC"
#   include "FunctionTracer.h"
#else
#    define CALLED(x...)
#    define TRACE(x...)
#endif

namespace WebCore {

FrameLoaderClientHaiku::FrameLoaderClientHaiku(BWebPage* webPage, BWebFrame* webFrame)
    : m_webPage(webPage)
    , m_webFrame(webFrame)
    , m_messenger()
    , m_loadingErrorPage(false)
    , m_pluginView(0)
    , m_hasSentResponseToPlugin(false)
{
    CALLED("BWebPage: %p, BWebFrame: %p", webPage, webFrame);
    ASSERT(m_webPage);
    ASSERT(m_webFrame);
}

void FrameLoaderClientHaiku::setDispatchTarget(const BMessenger& messenger)
{
    m_messenger = messenger;
}

BWebPage* FrameLoaderClientHaiku::page() const
{
    return m_webPage;
}

void FrameLoaderClientHaiku::frameLoaderDestroyed()
{
    CALLED();
    // No one else feels responsible for the BWebFrame instance that created us.
    // Through Shutdown(), we initiate the deletion sequence, after this method returns,
    // this object will be gone.
    m_webFrame->Shutdown();
}

bool FrameLoaderClientHaiku::hasWebView() const
{
    return m_webPage->WebView();
}

void FrameLoaderClientHaiku::makeRepresentation(DocumentLoader*)
{
    notImplemented();
}

void FrameLoaderClientHaiku::forceLayout()
{
    CALLED();

    Frame* frame = m_webFrame->Frame();
    if (frame->document() && frame->document()->inPageCache())
        return;

    FrameView* view = frame->view();
    if (view)
        view->forceLayout(true);
}

void FrameLoaderClientHaiku::forceLayoutForNonHTML()
{
    notImplemented();
}

void FrameLoaderClientHaiku::setCopiesOnScroll()
{
    // Other ports mention "apparently mac specific", but I believe it may have to
    // do with achieving that WebCore does not repaint the parts that we can scroll
    // by blitting.
    notImplemented();
}

void FrameLoaderClientHaiku::detachedFromParent2()
{
}

void FrameLoaderClientHaiku::detachedFromParent3()
{
}

bool FrameLoaderClientHaiku::dispatchDidLoadResourceFromMemoryCache(DocumentLoader* loader,
                                                                    const ResourceRequest& request,
                                                                    const ResourceResponse& response,
                                                                    int length)
{
    CALLED("DocumentLoader: %p, request: %s, length: %d", loader,
           request.url().string().utf8().data(), length);
    notImplemented();
    return false;
}

void FrameLoaderClientHaiku::assignIdentifierToInitialRequest(unsigned long identifier,
                                                              DocumentLoader* loader,
                                                              const ResourceRequest& request)
{
//    CALLED("identifier: %u, DocumentLoader: %p", identifier, loader);
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchWillSendRequest(DocumentLoader* loader, unsigned long identifier,
                                                     ResourceRequest& request,
                                                     const ResourceResponse& redirectResponse)
{
    CALLED("DocumentLoader: %p, identifier: %u, request: %s, redirectResponse: %d", loader, identifier,
           request.url().string().utf8().data(), redirectResponse.url().string().utf8().data());
    notImplemented();
}

bool FrameLoaderClientHaiku::shouldUseCredentialStorage(DocumentLoader*, unsigned long)
{
    notImplemented();
    return false;
}

void FrameLoaderClientHaiku::dispatchDidReceiveAuthenticationChallenge(DocumentLoader*, unsigned long, const AuthenticationChallenge& challenge)
{
    const ProtectionSpace& space = challenge.protectionSpace();
    String text = "Host \"" + space.host() + "\" requests authentication for realm \"" + space.realm() + "\"\n";
    text += "Authentication Scheme: ";
    switch (space.authenticationScheme()) {
    case ProtectionSpaceAuthenticationSchemeHTTPBasic:
        text += "Basic (data will be sent as plain text)";
        break;
    case ProtectionSpaceAuthenticationSchemeHTTPDigest:
        text += "Digest (data will not be sent plain text)";
        break;
    default:
        text += "Unknown (possibly plaintext)";
        break;
    }

    BMessage challengeMessage(AUTHENTICATION_CHALLENGE);
    challengeMessage.AddString("text", text);
    challengeMessage.AddString("user", challenge.proposedCredential().user());
    challengeMessage.AddString("password", challenge.proposedCredential().password());
    challengeMessage.AddUInt32("failureCount", challenge.previousFailureCount());
    challengeMessage.AddPointer("view", m_webPage->WebView());

    BMessage authenticationReply;
    m_messenger.SendMessage(&challengeMessage, &authenticationReply);

    BString user;
    BString password;
    if (authenticationReply.FindString("user", &user) != B_OK
        || authenticationReply.FindString("password", &password) != B_OK) {
        challenge.authenticationClient()->receivedCancellation(challenge);
    } else {
        if (!user.Length() && !password.Length())
            challenge.authenticationClient()->receivedRequestToContinueWithoutCredential(challenge);
        else {
            bool rememberCredentials = false;
            CredentialPersistence persistence = CredentialPersistenceForSession;
            if (authenticationReply.FindBool("rememberCredentials",
                &rememberCredentials) == B_OK && rememberCredentials) {
                persistence = CredentialPersistencePermanent;
            }

            Credential credential(user.String(), password.String(), persistence);
            challenge.authenticationClient()->receivedCredential(challenge, credential);
        }
    }
}

void FrameLoaderClientHaiku::dispatchDidCancelAuthenticationChallenge(DocumentLoader*, unsigned long, const AuthenticationChallenge&)
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidReceiveResponse(DocumentLoader* loader,
                                                        unsigned long identifier,
                                                        const ResourceResponse& response)
{
//    CALLED("DocumentLoader: %p, identifier: %u, response: %d", loader, identifier,
//           response.url().string().utf8().data());
    m_response = response;
}

void FrameLoaderClientHaiku::dispatchDidReceiveContentLength(DocumentLoader* loader,
                                                             unsigned long id, int length)
{
//    CALLED("DocumentLoader: %p, identifier: %u, length: %d", loader, id, length);
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidFinishLoading(DocumentLoader* loader, unsigned long identifier)
{
    CALLED("DocumentLoader: %p, identifier: %u", loader, identifier);
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidFailLoading(DocumentLoader* loader, unsigned long, const ResourceError& error)
{
    if (error.isCancellation())
        return;
    BMessage message(LOAD_FAILED);
    message.AddString("url", loader->url().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidHandleOnloadEvents()
{
    CALLED();
    BMessage message(LOAD_ONLOAD_HANDLE);
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidReceiveServerRedirectForProvisionalLoad()
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidCancelClientRedirect()
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchWillPerformClientRedirect(const KURL&, double interval, double fireDate)
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidChangeLocationWithinPage()
{
    BMessage message(LOAD_DOC_COMPLETED);
    message.AddString("url", m_webFrame->Frame()->document()->url().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidPushStateWithinPage()
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidReplaceStateWithinPage()
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidPopStateWithinPage()
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchWillClose()
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidReceiveIcon()
{
    if (m_loadingErrorPage)
        return;

    BMessage message(ICON_RECEIVED);
    message.AddString("url", m_webFrame->Frame()->document()->url().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidStartProvisionalLoad()
{
    CALLED();
    if (m_loadingErrorPage) {
        TRACE("m_loadingErrorPage\n");
        m_loadingErrorPage = false;
    }

    BMessage message(LOAD_NEGOTIATING);
    message.AddString("url", m_webFrame->Frame()->loader()->provisionalDocumentLoader()->request().url().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidReceiveTitle(const StringWithDirection& title)
{
    CALLED();
    if (m_loadingErrorPage) {
        TRACE("m_loadingErrorPage\n");
        return;
    }

    m_webFrame->SetTitle(title.string());

    BMessage message(TITLE_CHANGED);
    message.AddString("title", title.string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidChangeIcons(WebCore::IconType)
{
    if (!m_webFrame)
        return;

    // FIXME: In order to get notified of icon URLS' changes, add a notification.
    // emit iconsChanged();
}

void FrameLoaderClientHaiku::dispatchDidCommitLoad()
{
    CALLED();
    if (m_loadingErrorPage) {
        TRACE("m_loadingErrorPage\n");
        return;
    }

    BMessage message(LOAD_COMMITTED);
    message.AddString("url", m_webFrame->Frame()->loader()->documentLoader()->request().url().string());
    dispatchMessage(message);

    // We should assume first the frame has no title. If it has, then the above
    // dispatchDidReceiveTitle() will be called very soon with the correct title.
    // This properly resets the title when we navigate to a URI without a title.
    BMessage titleMessage(TITLE_CHANGED);
    titleMessage.AddString("title", "");
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidFailProvisionalLoad(const ResourceError& error)
{
    dispatchDidFailLoad(error);
}

void FrameLoaderClientHaiku::dispatchDidFailLoad(const ResourceError& error)
{
    CALLED();
    if (m_loadingErrorPage) {
        TRACE("m_loadingErrorPage\n");
        return;
    }
    if (!shouldFallBack(error)) {
        TRACE("should not fall back\n");
        return;
    }

    m_loadingErrorPage = true;

    // NOTE: This could be used to display the error right in the page. However, I find
    // the error alert somehow better to manage. For example, on a partial load error,
    // at least some content stays readable if we don't overwrite it with the error text... :-)
//    BString content("<html><body>");
//    content << error.localizedDescription().utf8().data();
//    content << "</body></html>";
//
//    m_webFrame->SetFrameSource(content);
}

void FrameLoaderClientHaiku::dispatchDidFinishDocumentLoad()
{
    BMessage message(LOAD_DOC_COMPLETED);
    message.AddString("url", m_webFrame->Frame()->document()->url().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidFinishLoad()
{
    CALLED();
    if (m_loadingErrorPage) {
        m_loadingErrorPage = false;
        TRACE("m_loadingErrorPage\n");
        return;
    }

    BMessage message(LOAD_FINISHED);
    message.AddString("url", m_webFrame->Frame()->document()->url().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::dispatchDidFirstLayout()
{
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDidFirstVisuallyNonEmptyLayout()
{
    if (m_loadingErrorPage)
        return;

    notImplemented();
}

void FrameLoaderClientHaiku::dispatchWillSubmitForm(FramePolicyFunction function, PassRefPtr<FormState>)
{
    CALLED();
    notImplemented();
    // FIXME: Send an event to allow for alerts and cancellation.
    callPolicyFunction(function, PolicyUse);
}

void FrameLoaderClientHaiku::dispatchDidLoadMainResource(DocumentLoader*)
{
    notImplemented();
}

Frame* FrameLoaderClientHaiku::dispatchCreatePage(const NavigationAction& /*action*/)
{
    CALLED();
    WebCore::Page* page = m_webPage->createNewPage();
    if (page)
        return page->mainFrame();

    return 0;
}

void FrameLoaderClientHaiku::dispatchShow()
{
    CALLED();
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchDecidePolicyForResponse(FramePolicyFunction function, const WebCore::ResourceResponse& response, const WebCore::ResourceRequest& request)
{
    if (request.isNull()) {
printf("FrameLoaderClientHaiku::dispatchDecidePolicyForResponse %s -> ignore (isNull)\n",
BString(response.mimeType()).String());
        callPolicyFunction(function, PolicyIgnore);
        return;
    }
    // we need to call directly here
    if (canShowMIMEType(response.mimeType())) {
        callPolicyFunction(function, PolicyUse);
    } else if (!request.url().isLocalFile() && response.mimeType() != "application/x-shockwave-flash") {
printf("FrameLoaderClientHaiku::dispatchDecidePolicyForResponse %s -> download\n",
BString(response.mimeType()).String());
        callPolicyFunction(function, PolicyDownload);
    } else {
printf("FrameLoaderClientHaiku::dispatchDecidePolicyForResponse %s -> ignore (local URL or Flash)\n",
BString(response.mimeType()).String());
        callPolicyFunction(function, PolicyIgnore);
    }
}

void FrameLoaderClientHaiku::dispatchDecidePolicyForNewWindowAction(FramePolicyFunction function,
    const NavigationAction& action, const ResourceRequest& request, PassRefPtr<FormState> formState, const String& targetName)
{
    ASSERT(function);
    if (!function)
        return;

    // Ignore null requests as the Gtk port does. The Qt port doesn't and I have
    // one URL where Arora crashes the same as WebPositive if not checked here.
    if (request.isNull()) {
        callPolicyFunction(function, PolicyIgnore);
        return;
    }

    if (!m_messenger.IsValid() || !isTertiaryMouseButton(action)) {
        dispatchNavigationRequested(request);
        callPolicyFunction(function, PolicyUse);
        return;
    }

    // NOTE: This is what the Qt port does in QWebPage::acceptNavigationRequest() if the
    // current delegation policy is "DelegateExternalLinks". Must be good for something.
    if (WebCore::SchemeRegistry::shouldTreatURLSchemeAsLocal(request.url().protocol())) {
        dispatchNavigationRequested(request);
        callPolicyFunction(function, PolicyUse);
        return;
    }

    // Clicks with the tertiary mouse button shall open a new window,
    // (or tab respectively depending on browser) - *ignore* the request for this page
    // then, since we create it ourself.
    BMessage message(NEW_WINDOW_REQUESTED);
    message.AddString("url", request.url().string());
    // Don't switch to the new tab, but load it in the background.
    message.AddBool("primary", false);
    dispatchMessage(message);

    if (action.type() == NavigationTypeFormSubmitted || action.type() == NavigationTypeFormResubmitted)
        m_webFrame->Frame()->loader()->resetMultipleFormSubmissionProtection();

    if (action.type() == NavigationTypeLinkClicked) {
        ResourceRequest emptyRequest;
        m_webFrame->Frame()->loader()->activeDocumentLoader()->setLastCheckedRequest(emptyRequest);
    }

    callPolicyFunction(function, PolicyIgnore);
}

void FrameLoaderClientHaiku::dispatchDecidePolicyForNavigationAction(FramePolicyFunction function,
    const NavigationAction& action, const ResourceRequest& request, PassRefPtr<FormState> formState)
{
    // Potentially we want to open a new window, when the user clicked with the
    // tertiary mouse button. That's why we can reuse the other method.
    dispatchDecidePolicyForNewWindowAction(function, action, request, formState, String());
}

void FrameLoaderClientHaiku::cancelPolicyCheck()
{
    CALLED();
    notImplemented();
}

void FrameLoaderClientHaiku::dispatchUnableToImplementPolicy(const ResourceError&)
{
    CALLED();
    notImplemented();
}

void FrameLoaderClientHaiku::revertToProvisionalState(DocumentLoader*)
{
    CALLED();
    notImplemented();
}

void FrameLoaderClientHaiku::setMainDocumentError(WebCore::DocumentLoader* loader, const WebCore::ResourceError& error)
{
    CALLED();
    if (m_pluginView) {
        m_pluginView->didFail(error);
        m_pluginView = 0;
        m_hasSentResponseToPlugin = false;
    }

    if (error.isCancellation())
        return;

    BMessage message(MAIN_DOCUMENT_ERROR);
    message.AddString("url", error.failingURL());
    message.AddString("error", error.localizedDescription());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::postProgressStartedNotification()
{
    if (m_webFrame && m_webFrame->Frame()->page()) {
        // A new load starts, so lets clear the previous error.
//        m_loadError = ResourceError();
        postProgressEstimateChangedNotification();
    }
    if (!m_webFrame || m_webFrame->Frame()->tree()->parent())
        return;
    triggerNavigationHistoryUpdate();
}

void FrameLoaderClientHaiku::postProgressEstimateChangedNotification()
{
    m_webPage->setLoadingProgress(m_webFrame->Frame()->page()->progress()->estimatedProgress() * 100);

    // Triggering this continually during loading progress makes stopping more reliably available.
    triggerNavigationHistoryUpdate();
}

void FrameLoaderClientHaiku::postProgressFinishedNotification()
{
    BMessage message(LOAD_DL_COMPLETED);
    message.AddString("url", m_webFrame->Frame()->document()->url().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::setMainFrameDocumentReady(bool)
{
    notImplemented();
    // this is only interesting once we provide an external API for the DOM
}

void FrameLoaderClientHaiku::download(ResourceHandle* handle, const ResourceRequest& request, const ResourceResponse& response)
{
    m_webPage->requestDownload(handle, request, response);
}

void FrameLoaderClientHaiku::startDownload(const ResourceRequest& request, const String& /*suggestedName*/)
{
    m_webPage->requestDownload(request);
}

void FrameLoaderClientHaiku::willChangeTitle(DocumentLoader*)
{
    // We act in didChangeTitle
}

void FrameLoaderClientHaiku::didChangeTitle(DocumentLoader* docLoader)
{
    setTitle(docLoader->title(), docLoader->url());
}

void FrameLoaderClientHaiku::committedLoad(WebCore::DocumentLoader* loader, const char* data, int length)
{
    CALLED();

    if (!m_pluginView) {
        ASSERT(loader->frame());
        loader->commitData(data, length);

        Frame* coreFrame = loader->frame();
        if (coreFrame && coreFrame->document()->isMediaDocument())
            loader->cancelMainResourceLoad(coreFrame->loader()->client()->pluginWillHandleLoadError(loader->response()));
    }

    // We re-check here as the plugin can have been created.
    if (m_pluginView && m_pluginView->isPluginView()) {
        if (!m_hasSentResponseToPlugin) {
            m_pluginView->didReceiveResponse(loader->response());
            // The function didReceiveResponse sets up a new stream to the plug-in.
            // On a full-page plug-in, a failure in setting up this stream can cause the
            // main document load to be cancelled, setting m_pluginView to null.
            if (!m_pluginView)
                return;
            m_hasSentResponseToPlugin = true;
        }
        m_pluginView->didReceiveData(data, length);
    }
}

void FrameLoaderClientHaiku::finishedLoading(DocumentLoader* documentLoader)
{
    CALLED();

    if (!m_pluginView) {
        TRACE("!m_pluginView\n");
        documentLoader->writer()->setEncoding(m_response.textEncodingName(), false);
    } else {
        TRACE("m_pluginView\n");
        m_pluginView->didFinishLoading();
        m_pluginView = 0;
        m_hasSentResponseToPlugin = false;
    }
}

void FrameLoaderClientHaiku::updateGlobalHistory()
{
    WebCore::Frame* frame = m_webFrame->Frame();
    if (!frame)
        return;

    BMessage message(UPDATE_HISTORY);
    message.AddString("url", frame->loader()->documentLoader()->urlForHistory().string());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::updateGlobalHistoryRedirectLinks()
{
    updateGlobalHistory();
}

bool FrameLoaderClientHaiku::shouldGoToHistoryItem(WebCore::HistoryItem*) const
{
    // FIXME: Should probably be refuse to go to the item when it contains
    // form data that has already been sent or something.
    return true;
}

bool FrameLoaderClientHaiku::shouldStopLoadingForHistoryItem(WebCore::HistoryItem*) const
{
    return true;
}

void FrameLoaderClientHaiku::saveScrollPositionAndViewStateToItem(WebCore::HistoryItem*)
{
    // NOTE: I think we don't have anything to do here, since it a) obviously works already
    // and b) it is probably for the case when using a native ScrollView as widget, which
    // we don't do.
    notImplemented();
}

void FrameLoaderClientHaiku::didDisplayInsecureContent()
{
}

void FrameLoaderClientHaiku::didRunInsecureContent(WebCore::SecurityOrigin*, const WebCore::KURL&)
{
    notImplemented();
}

void FrameLoaderClientHaiku::didDetectXSS(const KURL&, bool)
{
    notImplemented();
}

WebCore::ResourceError FrameLoaderClientHaiku::cancelledError(const WebCore::ResourceRequest& request)
{
    CALLED();
    notImplemented();
    ResourceError error = ResourceError(String(), WebKitErrorCannotShowURL,
                                        request.url().string(), "Load request cancelled");
    error.setIsCancellation(true);
    return error;
}

WebCore::ResourceError FrameLoaderClientHaiku::blockedError(const ResourceRequest& request)
{
    CALLED();
    notImplemented();
    return ResourceError(String(), WebKitErrorCannotUseRestrictedPort,
                         request.url().string(), "Not allowed to use restricted network port");
}

WebCore::ResourceError FrameLoaderClientHaiku::cannotShowURLError(const WebCore::ResourceRequest& request)
{
    CALLED();
    notImplemented();
    return ResourceError(String(), WebKitErrorCannotShowURL,
                         request.url().string(), "URL cannot be shown");
}

WebCore::ResourceError FrameLoaderClientHaiku::interruptedForPolicyChangeError(const WebCore::ResourceRequest& request)
{
    CALLED();
    notImplemented();
    ResourceError error = ResourceError(String(), WebKitErrorFrameLoadInterruptedByPolicyChange,
                                        request.url().string(), "Frame load was interrupted");
    error.setIsCancellation(true);
    return error;
}

WebCore::ResourceError FrameLoaderClientHaiku::cannotShowMIMETypeError(const WebCore::ResourceResponse& response)
{
    CALLED();
    notImplemented();
    // FIXME: This can probably be used to automatically close pages that have no content,
    // but only triggered a download. Since BWebPage is used for initiating a BWebDownload,
    // it could remember doing so and then we could ask here if we are the main frame,
    // have no content, but did download something -- then we could asked to be closed.
    return ResourceError(String(), WebKitErrorCannotShowMIMEType,
                         response.url().string(), "Content with the specified MIME type cannot be shown");
}

WebCore::ResourceError FrameLoaderClientHaiku::fileDoesNotExistError(const WebCore::ResourceResponse& response)
{
    CALLED();
    notImplemented();
    return ResourceError(String(), WebKitErrorCannotShowURL,
                         response.url().string(), "File does not exist");
}

ResourceError FrameLoaderClientHaiku::pluginWillHandleLoadError(const ResourceResponse& response)
{
    CALLED();
    notImplemented();
    return ResourceError(String(), WebKitErrorPlugInWillHandleLoad,
                         response.url().string(), "Plugin will handle load");
}

bool FrameLoaderClientHaiku::shouldFallBack(const WebCore::ResourceError& error)
{
    return !(error.isCancellation()
             || error.errorCode() == WebKitErrorFrameLoadInterruptedByPolicyChange
             || error.errorCode() == WebKitErrorPlugInWillHandleLoad);
}

bool FrameLoaderClientHaiku::canHandleRequest(const WebCore::ResourceRequest&) const
{
    return true;
}

bool FrameLoaderClientHaiku::canShowMIMEType(const String& mimeType) const
{
    CALLED("%s", mimeType.utf8().data());
    // FIXME: Usually, the mime type will have been detexted. This is supposed to work around
    // downloading some empty files, that can be observed.
    if (!mimeType.length())
        return true;

    if (MIMETypeRegistry::isSupportedImageMIMEType(mimeType))
        return true;

    if (MIMETypeRegistry::isSupportedNonImageMIMEType(mimeType))
        return true;

    Frame* frame = m_webFrame->Frame();
    if (frame && frame->settings() && frame->settings()->arePluginsEnabled()
        && PluginDatabase::installedPlugins()->isMIMETypeRegistered(mimeType))
        return true;

    return false;
}

bool FrameLoaderClientHaiku::canShowMIMETypeAsHTML(const String& MIMEType) const
{
    notImplemented();
    return false;
}

bool FrameLoaderClientHaiku::representationExistsForURLScheme(const String& URLScheme) const
{
    notImplemented();
    return false;
}

String FrameLoaderClientHaiku::generatedMIMETypeForURLScheme(const String& URLScheme) const
{
    notImplemented();
    return String();
}

void FrameLoaderClientHaiku::frameLoadCompleted()
{
}

void FrameLoaderClientHaiku::saveViewStateToItem(HistoryItem*)
{
    notImplemented();
}

void FrameLoaderClientHaiku::restoreViewState()
{
    // This seems unimportant, the Qt port mentions this for it's corresponding signal:
    //   "This signal is emitted when the load of \a frame is finished and the application
    //   may now update its state accordingly."
    // Could be this is important for ports which use actual platform widgets.
    notImplemented();
}

void FrameLoaderClientHaiku::provisionalLoadStarted()
{
    notImplemented();
}

void FrameLoaderClientHaiku::didFinishLoad()
{
    notImplemented();
}

void FrameLoaderClientHaiku::prepareForDataSourceReplacement()
{
    notImplemented();
}

WTF::PassRefPtr<DocumentLoader> FrameLoaderClientHaiku::createDocumentLoader(const ResourceRequest& request, const SubstituteData& substituteData)
{
    CALLED("request: %s", request.url().string().utf8().data());
    RefPtr<DocumentLoader> loader = DocumentLoader::create(request, substituteData);
// NOTE: The Gtk port does not do this, so I commented it out for now:
//    if (substituteData.isValid())
//        loader->setDeferMainResourceDataLoad(false);
    return loader.release();
}

void FrameLoaderClientHaiku::setTitle(const StringWithDirection&, const KURL&)
{
    notImplemented();
}

void FrameLoaderClientHaiku::savePlatformDataToCachedFrame(CachedFrame* cachedPage)
{
    CALLED();
    // Nothing to be done here for the moment. We don't associate any platform data
}

void FrameLoaderClientHaiku::transitionToCommittedFromCachedFrame(CachedFrame* cachedFrame)
{
    CALLED();
    ASSERT(cachedFrame->view());

    // FIXME: I guess we would have to restore platform data from the cachedFrame here,
    // data associated in savePlatformDataToCachedFrame().

    postCommitFrameViewSetup(m_webFrame, cachedFrame->view(), false);
}

void FrameLoaderClientHaiku::transitionToCommittedForNewPage()
{
    CALLED();
    ASSERT(m_webFrame);

    Frame* frame = m_webFrame->Frame();

    BRect bounds = m_webPage->viewBounds();
    IntSize size = IntSize(bounds.IntegerWidth() + 1, bounds.IntegerHeight() + 1);

    bool transparent = m_webFrame->IsTransparent();
    Color backgroundColor = transparent ? WebCore::Color::transparent : WebCore::Color::white;

    frame->createView(size, backgroundColor, transparent, IntSize(), false);

    postCommitFrameViewSetup(m_webFrame, frame->view(), true);
}

String FrameLoaderClientHaiku::userAgent(const KURL&)
{
    BLanguage language;
    BLocale::Default()->GetLanguage(&language);
    BString languageTag(language.Code());
    if (language.CountryCode() != NULL)
        languageTag << "-" << language.CountryCode();

    // FIXME: Get the app name from the app. Hardcoded WebPositive for now. Mentioning "Safari" is needed for some sites like gmail.com.
    BString userAgent("Mozilla/5.0 (compatible; U; Haiku x86; %language%) AppleWebKit/%webkit% (KHTML, like Gecko) Haiku/R1 WebPositive/1.1 Safari/%webkit%");
    userAgent.ReplaceAll("%webkit%", WebKitInfo::WebKitVersion().String());
    userAgent.ReplaceAll("%language%", languageTag.String());
    return userAgent;
}

bool FrameLoaderClientHaiku::canCachePage() const
{
    return true;
}

PassRefPtr<Frame> FrameLoaderClientHaiku::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement,
    const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight)
{
    CALLED();
    // FIXME: We should apply the right property to the frameView. (scrollbar,margins)
    ASSERT(m_webFrame);

    WebFramePrivate* data = new WebFramePrivate;
    data->requestedURL = url.string();
    data->name = name;
    data->ownerElement = ownerElement;
    data->page = m_webPage->page();

    BWebFrame* frame = new BWebFrame(m_webPage, m_webFrame, data);
    // The ownership of "frame" is implicitely transferred to the FrameLoadClientHaiku
    // instance which is created in the BWebFrame consructor.

    // As long as we don't return the Frame, we are responsible for deleting it.
    RefPtr<Frame> childFrame = frame->Frame();

    // The creation of the frame may have run arbitrary JavaScript that removed it from the page already.
    if (!childFrame->page())
        return 0;

    childFrame->loader()->loadURLIntoChildFrame(url, referrer, childFrame.get());

    // The frame's onload handler may have removed it from the document.
    if (!childFrame->tree()->parent())
        return 0;

    return childFrame.release();
}

void FrameLoaderClientHaiku::didTransferChildFrameToNewDocument()
{
    CALLED();
}

ObjectContentType FrameLoaderClientHaiku::objectContentType(const KURL& url, const String& originalMimeType, bool /*shouldPreferPlugInsForImages*/)
{
    CALLED();
    if (url.isEmpty() && !originalMimeType.length())
        return ObjectContentNone;

    String mimeType = originalMimeType;
    if (!mimeType.length()) {
        entry_ref ref;
        if (get_ref_for_path(url.path().utf8().data(), &ref) == B_OK) {
            BMimeType type;
            if (BMimeType::GuessMimeType(&ref, &type) == B_OK)
                mimeType = type.Type();
        }
    }

    if (!mimeType.length())
        return ObjectContentFrame;

    if (MIMETypeRegistry::isSupportedImageMIMEType(mimeType))
        return ObjectContentImage;

    if (PluginDatabase::installedPlugins()->isMIMETypeRegistered(mimeType))
        return ObjectContentNetscapePlugin;

    if (MIMETypeRegistry::isSupportedNonImageMIMEType(mimeType))
        return ObjectContentFrame;

    if (url.protocol() == "about")
        return ObjectContentFrame;

    return ObjectContentNone;
}

PassRefPtr<Widget> FrameLoaderClientHaiku::createPlugin(const IntSize&, HTMLPlugInElement*, const KURL&, const Vector<String>&,
                                                        const Vector<String>&, const String&, bool loadManually)
{
    CALLED();
    notImplemented();
    return 0;
}

void FrameLoaderClientHaiku::redirectDataToPlugin(Widget* pluginWidget)
{
    CALLED();
    ASSERT(!m_pluginView);
    m_pluginView = static_cast<PluginView*>(pluginWidget);
    m_hasSentResponseToPlugin = false;
}

PassRefPtr<Widget> FrameLoaderClientHaiku::createJavaAppletWidget(const IntSize&, HTMLAppletElement*, const KURL& baseURL,
                                                                  const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    notImplemented();
    return 0;
}

String FrameLoaderClientHaiku::overrideMediaType() const
{
    notImplemented();
    return String();
}

void FrameLoaderClientHaiku::didSaveToPageCache()
{
}

void FrameLoaderClientHaiku::didRestoreFromPageCache()
{
}

void FrameLoaderClientHaiku::dispatchDidBecomeFrameset(bool)
{
}

void FrameLoaderClientHaiku::dispatchDidClearWindowObjectInWorld(DOMWrapperWorld* world)
{
    if (world != mainThreadNormalWorld())
        return;

    if (m_webFrame) {
        BMessage message(JAVASCRIPT_WINDOW_OBJECT_CLEARED);
        dispatchMessage(message);
    }
}

void FrameLoaderClientHaiku::documentElementAvailable()
{
}

void FrameLoaderClientHaiku::registerForIconNotification(bool listen)
{
    notImplemented();
}

PassRefPtr<FrameNetworkingContext> FrameLoaderClientHaiku::createNetworkingContext()
{
    notImplemented();
    return 0;
}

void FrameLoaderClientHaiku::didPerformFirstNavigation() const
{
    triggerNavigationHistoryUpdate();
}

// #pragma mark - private

void FrameLoaderClientHaiku::callPolicyFunction(FramePolicyFunction function, PolicyAction action)
{
    (m_webFrame->Frame()->loader()->policyChecker()->*function)(action);
}

void FrameLoaderClientHaiku::triggerNavigationHistoryUpdate() const
{
    WebCore::Page* page = m_webFrame->Frame()->page();
    WebCore::FrameLoader* loader = m_webFrame->Frame()->loader();
    if (!page || !loader)
        return;
    BMessage message(UPDATE_NAVIGATION_INTERFACE);
    message.AddBool("can go backward", page->canGoBackOrForward(-1));
    message.AddBool("can go forward", page->canGoBackOrForward(1));
    message.AddBool("can stop", loader->isLoading());
    dispatchMessage(message);
}

void FrameLoaderClientHaiku::postCommitFrameViewSetup(BWebFrame* frame, FrameView* view, bool resetValues) const
{
    view->setTopLevelPlatformWidget(m_webPage->WebView());
}

bool FrameLoaderClientHaiku::isTertiaryMouseButton(const NavigationAction& action) const
{
    if (action.event() && action.event()->isMouseEvent()) {
        const MouseEvent* mouseEvent = dynamic_cast<const MouseEvent*>(action.event());
        return (mouseEvent && mouseEvent->button() == 1);
    }
    return false;
}

status_t FrameLoaderClientHaiku::dispatchNavigationRequested(const ResourceRequest& request) const
{
    BMessage message(NAVIGATION_REQUESTED);
    message.AddString("url", request.url().string());
    return dispatchMessage(message);
}

status_t FrameLoaderClientHaiku::dispatchMessage(BMessage& message) const
{
    message.AddPointer("view", m_webPage->WebView());
    return m_messenger.SendMessage(&message);
}

} // namespace WebCore

