/*
 * Copyright (C) 2009 Maxime Simon <simon.maxime@gmail.com>
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
#include "LocalizedStrings.h"

#include "NotImplemented.h"
#include "PlatformString.h"


namespace WebCore {
String submitButtonDefaultLabel()
{
    return "Submit";
}

String inputElementAltText()
{
    return String();
}

String resetButtonDefaultLabel()
{
    return "Reset";
}

String searchableIndexIntroduction()
{
    return "Searchable Index";
}

String fileButtonChooseFileLabel()
{
    return "Choose File";
}

String fileButtonChooseMultipleFilesLabel()
{
    return "Choose Files";
}

String fileButtonNoFileSelectedLabel()
{
    return "No file selected";
}

String fileButtonNoFilesSelectedLabel()
{
    return "No files selected";
}

String contextMenuItemTagOpenLinkInNewWindow()
{
    return "Open link in new tab";
}

String contextMenuItemTagDownloadLinkToDisk()
{
    return "Download link to disk";
}

String contextMenuItemTagCopyLinkToClipboard()
{
    return "Copy link to clipboard";
}

String contextMenuItemTagOpenImageInNewWindow()
{
    return "Open image in new window";
}

String contextMenuItemTagDownloadImageToDisk()
{
    return "Download image to disk";
}

String contextMenuItemTagCopyImageToClipboard()
{
    return "Copy image to clipboard";
}

String contextMenuItemTagOpenFrameInNewWindow()
{
    return "Open frame in new window";
}

String contextMenuItemTagCopy()
{
    return "Copy";
}

String contextMenuItemTagGoBack()
{
    return "Go back";
}

String contextMenuItemTagGoForward()
{
    return "Go forward";
}

String contextMenuItemTagStop()
{
    return "Stop";
}

String contextMenuItemTagReload()
{
    return "Reload";
}

String contextMenuItemTagCut()
{
    return "Cut";
}

String contextMenuItemTagPaste()
{
    return "Paste";
}

String contextMenuItemTagNoGuessesFound()
{
    return "No guesses found";
}

String contextMenuItemTagIgnoreSpelling()
{
    return "Ignore spelling";
}

String contextMenuItemTagLearnSpelling()
{
    return "Learn spelling";
}

String contextMenuItemTagSearchWeb()
{
    return "Search web";
}

String contextMenuItemTagLookUpInDictionary()
{
    return "Lookup in dictionary";
}

String contextMenuItemTagOpenLink()
{
    return "Open link";
}

String contextMenuItemTagIgnoreGrammar()
{
    return "Ignore grammar";
}

String contextMenuItemTagSpellingMenu()
{
    return "Spelling menu";
}

String contextMenuItemTagShowSpellingPanel(bool show)
{
    return "Show spelling panel";
}

String contextMenuItemTagCheckSpelling()
{
    return "Check spelling";
}

String contextMenuItemTagCheckSpellingWhileTyping()
{
    return "Check spelling while typing";
}

String contextMenuItemTagCheckGrammarWithSpelling()
{
    return "Check for grammar with spelling";
}

String contextMenuItemTagFontMenu()
{
    return "Font menu";
}

String contextMenuItemTagBold()
{
    return "Bold";
}

String contextMenuItemTagItalic()
{
    return "Italic";
}

String contextMenuItemTagUnderline()
{
    return "Underline";
}

String contextMenuItemTagOutline()
{
    return "Outline";
}

String contextMenuItemTagWritingDirectionMenu()
{
    return "Writing direction menu";
}

String contextMenuItemTagDefaultDirection()
{
    return "Default direction";
}

String contextMenuItemTagLeftToRight()
{
    return "Left to right";
}

String contextMenuItemTagRightToLeft()
{
    return "Right to left";
}

String contextMenuItemTagInspectElement()
{
    return "Inspect";
}

String contextMenuItemTagOpenVideoInNewWindow()
{
    return "Open Video in New Window";
}

String contextMenuItemTagOpenAudioInNewWindow()
{
    return "Open Audio in New Window";
}

String contextMenuItemTagCopyVideoLinkToClipboard()
{
    return "Copy Video Link Location";
}

String contextMenuItemTagCopyAudioLinkToClipboard()
{
    return "Copy Audio Link Location";
}

String contextMenuItemTagToggleMediaControls()
{
    return "Toggle Media Controls";
}

String contextMenuItemTagToggleMediaLoop()
{
    return "Toggle Media Loop Playback";
}

String contextMenuItemTagEnterVideoFullscreen()
{
    return "Switch Video to Fullscreen";
}

String contextMenuItemTagMediaPlay()
{
    return "Play";
}

String contextMenuItemTagMediaPause()
{
    return "Pause";
}

String contextMenuItemTagMediaMute()
{
    return "Mute";
}

String searchMenuNoRecentSearchesText()
{
    return "No recent text searches";
}

String searchMenuRecentSearchesText()
{
    return "Recent text searches";
}

String searchMenuClearRecentSearchesText()
{
    return "Clear recent text searches";
}

String unknownFileSizeText()
{
    return "Unknown";
}

String AXWebAreaText()
{
    return String();
}

String AXLinkText()
{
    return String();
}

String AXListMarkerText()
{
    return String();
}

String AXImageMapText()
{
    return String();
}

String AXHeadingText()
{
    return String();
}

String AXMenuListPopupActionVerb()
{
    return String();
}

String AXMenuListActionVerb()
{
    return String();
}

String missingPluginText()
{
    return "Missing Plug-in";
}

String crashedPluginText()
{
    return "Plug-in Failure";
}

String multipleFileUploadText(unsigned)
{
    // FIXME:
    notImplemented();
    return String();
}


String imageTitle(const String& filename, const IntSize& size)
{
    return String(filename);
}

String contextMenuItemTagTextDirectionMenu()
{
    return String();
}

String AXButtonActionVerb()
{
    return String();
}

String AXTextFieldActionVerb()
{
    return String();
}

String AXRadioButtonActionVerb()
{
    return String();
}

String AXCheckedCheckBoxActionVerb()
{
    return String();
}

String AXUncheckedCheckBoxActionVerb()
{
    return String();
}

String AXLinkActionVerb()
{
    return String();
}

String AXDefinitionListTermText()
{
    return String();
}

String AXDefinitionListDefinitionText()
{
    return String();
}

String validationMessageValueMissingText()
{
    notImplemented();
    return String();
}

String validationMessageValueMissingForCheckboxText()
{
    notImplemented();
    return String();
}

String validationMessageValueMissingForFileText()
{
    notImplemented();
    return String();
}

String validationMessageValueMissingForMultipleFileText()
{
    notImplemented();
    return String();
}

String validationMessageValueMissingForRadioText()
{
    notImplemented();
    return String();
}

String validationMessageValueMissingForSelectText()
{
    notImplemented();
    return String();
}

String validationMessageTypeMismatchText()
{
    notImplemented();
    return String();
}

String validationMessageTypeMismatchForEmailText()
{
    notImplemented();
    return String();
}

String validationMessageTypeMismatchForMultipleEmailText()
{
    notImplemented();
    return String();
}

String validationMessageTypeMismatchForURLText()
{
    notImplemented();
    return String();
}

String validationMessagePatternMismatchText()
{
    notImplemented();
    return String();
}

String validationMessageTooLongText(int valueLength, int maxLength)
{
    notImplemented();
    return String();
}

String validationMessageRangeUnderflowText(const String& minimum)
{
    notImplemented();
    return String();
}

String validationMessageRangeOverflowText(const String& maximum)
{
    notImplemented();
    return String();
}

String validationMessageStepMismatchText(const String& base, const String& step)
{
    notImplemented();
    return String();
}

} // namespace WebCore

