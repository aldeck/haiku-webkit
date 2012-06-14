/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007 Ryan Leavengood <leavengood@gmail.com>
 * Copyright (C) 2007 Andrea Anzani <andrea.anzani@gmail.com>
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
#include "EditorClientHaiku.h"

#include "CString.h"
#include "Document.h"
#include "EditCommand.h"
#include "Editor.h"
#include "FocusController.h"
#include "Frame.h"
#include "KeyboardEvent.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformKeyboardEvent.h"
#include "Settings.h"
#include "WebFrame.h"
#include "WebPage.h"
#include "WindowsKeyboardCodes.h"

namespace WebCore {

EditorClientHaiku::EditorClientHaiku(BWebPage* page)
    : m_page(page)
    , m_editing(false)
    , m_isInRedo(false)
{
}

void EditorClientHaiku::pageDestroyed()
{
    delete this;
}

bool EditorClientHaiku::shouldDeleteRange(Range*)
{
    notImplemented();
    return true;
}

bool EditorClientHaiku::shouldShowDeleteInterface(HTMLElement*)
{
    notImplemented();
    return false;
}

bool EditorClientHaiku::smartInsertDeleteEnabled()
{
    notImplemented();
    return false;
}

bool EditorClientHaiku::isSelectTrailingWhitespaceEnabled()
{
    notImplemented();
    return false;
}

bool EditorClientHaiku::isContinuousSpellCheckingEnabled()
{
    notImplemented();
    return false;
}

void EditorClientHaiku::toggleContinuousSpellChecking()
{
    notImplemented();
}

bool EditorClientHaiku::isGrammarCheckingEnabled()
{
    notImplemented();
    return false;
}

void EditorClientHaiku::toggleGrammarChecking()
{
    notImplemented();
}

int EditorClientHaiku::spellCheckerDocumentTag()
{
    notImplemented();
    return 0;
}

bool EditorClientHaiku::isEditable()
{
	if (m_page->MainFrame())
        return m_page->MainFrame()->IsEditable();
    return false;
}

bool EditorClientHaiku::shouldBeginEditing(WebCore::Range*)
{
    notImplemented();
    return true;
}

bool EditorClientHaiku::shouldEndEditing(WebCore::Range*)
{
    notImplemented();
    return true;
}

bool EditorClientHaiku::shouldInsertNode(Node*, Range*, EditorInsertAction)
{
    notImplemented();
    return true;
}

bool EditorClientHaiku::shouldInsertText(const String&, Range*, EditorInsertAction)
{
    notImplemented();
    return true;
}

bool EditorClientHaiku::shouldChangeSelectedRange(Range* fromRange, Range* toRange,
                                                  EAffinity, bool stillSelecting)
{
    notImplemented();
    return true;
}

bool EditorClientHaiku::shouldApplyStyle(WebCore::CSSStyleDeclaration*,
                                      WebCore::Range*)
{
    notImplemented();
    return true;
}

bool EditorClientHaiku::shouldMoveRangeAfterDelete(Range*, Range*)
{
    notImplemented();
    return true;
}

void EditorClientHaiku::didBeginEditing()
{
    notImplemented();
    m_editing = true;
}

void EditorClientHaiku::respondToChangedContents()
{
    notImplemented();
}

void EditorClientHaiku::respondToChangedSelection()
{
    Frame* frame = m_page->page()->focusController()->focusedOrMainFrame();
    if (!frame->editor()->ignoreCompositionSelectionChange()) {
        // FIXME:
        // notify "micro focus shanged" event
    }

    notImplemented();
}

void EditorClientHaiku::didEndEditing()
{
    m_editing = false;
}

void EditorClientHaiku::didWriteSelectionToPasteboard()
{
    notImplemented();
}

void EditorClientHaiku::didSetSelectionTypesForPasteboard()
{
    notImplemented();
}

void EditorClientHaiku::registerCommandForUndo(WTF::PassRefPtr<WebCore::EditCommand> command)
{
    if (!m_isInRedo)
        redoStack.clear();
    undoStack.append(command);
}

void EditorClientHaiku::registerCommandForRedo(WTF::PassRefPtr<WebCore::EditCommand> command)
{
    redoStack.append(command);
}

void EditorClientHaiku::clearUndoRedoOperations()
{
    undoStack.clear();
    redoStack.clear();
}

bool EditorClientHaiku::canUndo() const
{
    return !undoStack.isEmpty();
}

bool EditorClientHaiku::canRedo() const
{
    return !redoStack.isEmpty();
}

void EditorClientHaiku::undo()
{
printf("EditorClientHaiku::undo()\n");
    if (canUndo()) {
        RefPtr<WebCore::EditCommand> command(*(--undoStack.end()));
        undoStack.remove(--undoStack.end());
        // unapply will call us back to push this command onto the redo stack.
        command->unapply();
    }
}

void EditorClientHaiku::redo()
{
printf("EditorClientHaiku::redo()\n");
    if (canRedo()) {
        RefPtr<WebCore::EditCommand> command(*(--redoStack.end()));
        redoStack.remove(--redoStack.end());

        ASSERT(!m_isInRedo);
        m_isInRedo = true;
        // reapply will call us back to push this command onto the undo stack.
        command->reapply();
        m_isInRedo = false;
    }
}

#if 0
static const unsigned CtrlKey = 1 << 0;
static const unsigned AltKey = 1 << 1;
static const unsigned ShiftKey = 1 << 2;

struct KeyDownEntry {
    unsigned virtualKey;
    unsigned modifiers;
    const char* name;
};

struct KeyPressEntry {
    unsigned charCode;
    unsigned modifiers;
    const char* name;
};

static const KeyDownEntry keyDownEntries[] = {
    { VK_LEFT,   0,                  "MoveLeft"                                    },
    { VK_LEFT,   ShiftKey,           "MoveLeftAndModifySelection"                  },
    { VK_LEFT,   CtrlKey,            "MoveWordLeft"                                },
    { VK_LEFT,   CtrlKey | ShiftKey, "MoveWordLeftAndModifySelection"              },
    { VK_RIGHT,  0,                  "MoveRight"                                   },
    { VK_RIGHT,  ShiftKey,           "MoveRightAndModifySelection"                 },
    { VK_RIGHT,  CtrlKey,            "MoveWordRight"                               },
    { VK_RIGHT,  CtrlKey | ShiftKey, "MoveWordRightAndModifySelection"             },
    { VK_UP,     0,                  "MoveUp"                                      },
    { VK_UP,     ShiftKey,           "MoveUpAndModifySelection"                    },
    { VK_PRIOR,  ShiftKey,           "MovePageUpAndModifySelection"                },
    { VK_DOWN,   0,                  "MoveDown"                                    },
    { VK_DOWN,   ShiftKey,           "MoveDownAndModifySelection"                  },
    { VK_NEXT,   ShiftKey,           "MovePageDownAndModifySelection"              },
    { VK_PRIOR,  0,                  "MovePageUp"                                  },
    { VK_NEXT,   0,                  "MovePageDown"                                },
    { VK_HOME,   0,                  "MoveToBeginningOfLine"                       },
    { VK_HOME,   ShiftKey,           "MoveToBeginningOfLineAndModifySelection"     },
    { VK_HOME,   CtrlKey,            "MoveToBeginningOfDocument"                   },
    { VK_HOME,   CtrlKey | ShiftKey, "MoveToBeginningOfDocumentAndModifySelection" },

    { VK_END,    0,                  "MoveToEndOfLine"                             },
    { VK_END,    ShiftKey,           "MoveToEndOfLineAndModifySelection"           },
    { VK_END,    CtrlKey,            "MoveToEndOfDocument"                         },
    { VK_END,    CtrlKey | ShiftKey, "MoveToEndOfDocumentAndModifySelection"       },

    { VK_BACK,   0,                  "DeleteBackward"                              },
    { VK_BACK,   ShiftKey,           "DeleteBackward"                              },
    { VK_DELETE, 0,                  "DeleteForward"                               },
    { VK_BACK,   CtrlKey,            "DeleteWordBackward"                          },
    { VK_DELETE, CtrlKey,            "DeleteWordForward"                           },

    { 'B',       CtrlKey,            "ToggleBold"                                  },
    { 'I',       CtrlKey,            "ToggleItalic"                                },

    { VK_ESCAPE, 0,                  "Cancel"                                      },
    { VK_OEM_PERIOD, CtrlKey,        "Cancel"                                      },
    { VK_TAB,    0,                  "InsertTab"                                   },
    { VK_TAB,    ShiftKey,           "InsertBacktab"                               },
    { VK_RETURN, 0,                  "InsertNewline"                               },
    { VK_RETURN, ShiftKey,           "InsertLineBreak"                             },
    { VK_RETURN, CtrlKey,            "InsertNewline"                               },
    { VK_RETURN, AltKey,             "InsertNewline"                               },
    { VK_RETURN, AltKey | ShiftKey,  "InsertNewline"                               },
};

static const KeyPressEntry keyPressEntries[] = {
    { '\t',   0,                  "InsertTab"                                   },
    { '\t',   ShiftKey,           "InsertBacktab"                               },
    { '\r',   0,                  "InsertNewline"                               },
    { '\r',   ShiftKey,           "InsertLineBreak"                             },
    { '\r',   CtrlKey,            "InsertNewline"                               },
    { '\r',   AltKey,             "InsertNewline"                               },
    { '\r',   AltKey | ShiftKey,  "InsertNewline"                               },
};

static const char* interpretEditorCommandKeyEvent(const KeyboardEvent* evt)
{
    ASSERT(evt->type() == eventNames().keydownEvent || evt->type() == eventNames().keypressEvent);

    static HashMap<int, const char*>* keyDownCommandsMap = 0;
    static HashMap<int, const char*>* keyPressCommandsMap = 0;

    if (!keyDownCommandsMap) {
        keyDownCommandsMap = new HashMap<int, const char*>;
        keyPressCommandsMap = new HashMap<int, const char*>;

        for (unsigned i = 0; i < sizeof(keyDownEntries) / sizeof(KeyDownEntry); i++)
            keyDownCommandsMap->set(keyDownEntries[i].modifiers << 16 | keyDownEntries[i].virtualKey, keyDownEntries[i].name);

        for (unsigned i = 0; i < sizeof(keyPressEntries) / sizeof(KeyPressEntry); i++)
            keyPressCommandsMap->set(keyPressEntries[i].modifiers << 16 | keyPressEntries[i].charCode, keyPressEntries[i].name);
    }

    unsigned modifiers = 0;
    if (evt->shiftKey())
        modifiers |= ShiftKey;
    if (evt->altKey())
        modifiers |= AltKey;
    if (evt->ctrlKey())
        modifiers |= CtrlKey;

    if (evt->type() == eventNames().keydownEvent) {
        int mapKey = modifiers << 16 | evt->keyCode();
        return mapKey ? keyDownCommandsMap->get(mapKey) : 0;
    }

    int mapKey = modifiers << 16 | evt->charCode();
    return mapKey ? keyPressCommandsMap->get(mapKey) : 0;
}
#endif

void EditorClientHaiku::handleKeyboardEvent(KeyboardEvent* event)
{
    const PlatformKeyboardEvent* platformEvent = event->keyEvent();
    if (!platformEvent || platformEvent->type() == PlatformKeyboardEvent::KeyUp)
        return;

	if (handleEditingKeyboardEvent(event, platformEvent)) {
	    event->setDefaultHandled();
	    return;
	}

    Frame* frame = m_page->page()->focusController()->focusedOrMainFrame();
    if (!frame)
        return;

#if 1
    switch (platformEvent->windowsVirtualKeyCode()) {
#if 0
// Handled in upper layer
    case VK_UP:
        frame->editor()->command("MoveUp").execute();
        break;
    case VK_DOWN:
        frame->editor()->command("MoveDown").execute();
        break;
    case VK_PRIOR:  // PageUp
        frame->editor()->command("MoveUpByPageAndModifyCaret").execute();
        break;
    case VK_NEXT:  // PageDown
        frame->editor()->command("MoveDownByPageAndModifyCaret").execute();
        break;
#endif
    default:
        if (platformEvent->ctrlKey()) {
        	switch (platformEvent->windowsVirtualKeyCode()) {
#if 0
// Handled in upper layer
		    case VK_HOME:
		        frame->editor()->command("MoveToBeginningOfDocument").execute();
		        break;
		    case VK_END:
                frame->editor()->command("MoveToEndOfDocument").execute();
		        break;
#endif
		    case VK_A:
	            frame->editor()->command("SelectAll").execute();
		        break;
		    case VK_C: case VK_X:
	            frame->editor()->command("Copy").execute();
		        break;
        	}
        } else
            return;
    }
    event->setDefaultHandled();
#else
    // Don't allow editor commands or text insertion for nodes that
    // cannot edit, unless we are in caret mode.
    if (!frame->editor()->canEdit() && !(frame->settings() && frame->settings()->caretBrowsingEnabled()))
        return;

    const char* editorCommandString = interpretEditorCommandKeyEvent(event);
    if (editorCommandString) {
        Editor::Command command = frame->editor()->command(editorCommandString);

#if 0
// TODO: This doesn't work correctly, since for example VK_RETURN ends up here as
// PlatformKeyboardEvent::RawKeyDown and isTextInsertion() on the other hand, so it
// doesn't get executed. Can't say if the error is somehow that it ends up being a
// RawKeyDown, or if this code (from GTK) is broken.
        // On editor commands from key down events, we only want to let the event bubble up to
        // the DOM if it inserts text. If it doesn't insert text (e.g. Tab that changes focus)
        // we just want WebKit to handle it immediately without a DOM event.
        if (platformEvent->type() == PlatformKeyboardEvent::RawKeyDown) {
            if (!command.isTextInsertion() && command.execute(event))
                event->setDefaultHandled();

            return;
        } else if (command.execute(event)) {
            event->setDefaultHandled();
            return;
        }
#endif
        if (command.execute(event)) {
            event->setDefaultHandled();
            return;
        }
    }

    // This is just a normal text insertion, so wait to execute the insertion
    // until a keypress event happens. This will ensure that the insertion will not
    // be reflected in the contents of the field until the keyup DOM event.
    if (event->type() == eventNames().keypressEvent) {

#define ENABLE_INPUT_METHOD_STUFF 0
#if ENABLE_INPUT_METHOD_STUFF
        if (m_pendingComposition.Length()) {
            frame->editor()->confirmComposition(m_pendingComposition);

            clearPendingIMData();
            event->setDefaultHandled();

        } else if (m_pendingPreedit.Length()) {
            String preeditString = m_pendingPreedit;

            // Don't use an empty preedit as it will destroy the current
            // selection, even if the composition is cancelled or fails later on.
            if (!preeditString.isEmpty()) {
                Vector<CompositionUnderline> underlines;
                underlines.append(CompositionUnderline(0, preeditString.length(), Color(0, 0, 0), false));
                frame->editor()->setComposition(preeditString, underlines, 0, 0);
            }

            clearPendingIMData();
            event->setDefaultHandled();

        } else {
#endif
            // Don't insert null or control characters as they can result in unexpected behaviour
            if (event->charCode() < ' ')
                return;

            // Don't insert anything if a modifier is pressed
            if (platformEvent->ctrlKey() || platformEvent->altKey())
                return;

            if (frame->editor()->insertText(platformEvent->text(), event))
                event->setDefaultHandled();
#if ENABLE_INPUT_METHOD_STUFF
        }
#endif
    }
#endif
}

void EditorClientHaiku::handleInputMethodKeydown(KeyboardEvent*)
{
    notImplemented();
}

void EditorClientHaiku::textFieldDidBeginEditing(Element*)
{
    m_editing = true;
}

void EditorClientHaiku::textFieldDidEndEditing(Element*)
{
    m_editing = false;
}

void EditorClientHaiku::textDidChangeInTextField(Element*)
{
}

bool EditorClientHaiku::doTextFieldCommandFromEvent(Element*, KeyboardEvent*)
{
    return false;
}

void EditorClientHaiku::textWillBeDeletedInTextField(Element*)
{
    notImplemented();
}

void EditorClientHaiku::textDidChangeInTextArea(Element*)
{
    notImplemented();
}

void EditorClientHaiku::ignoreWordInSpellDocument(const String&)
{
    notImplemented();
}

void EditorClientHaiku::learnWord(const String&)
{
    notImplemented();
}

void EditorClientHaiku::checkSpellingOfString(const UChar*, int, int*, int*)
{
    notImplemented();
}

String EditorClientHaiku::getAutoCorrectSuggestionForMisspelledWord(const String& misspelledWord)
{
    notImplemented();
    return String();
}

void EditorClientHaiku::checkGrammarOfString(const UChar*, int, Vector<GrammarDetail>&, int*, int*)
{
    notImplemented();
}

void EditorClientHaiku::updateSpellingUIWithGrammarString(const String&, const GrammarDetail&)
{
    notImplemented();
}

void EditorClientHaiku::updateSpellingUIWithMisspelledWord(const String&)
{
    notImplemented();
}

void EditorClientHaiku::showSpellingUI(bool)
{
    notImplemented();
}

bool EditorClientHaiku::spellingUIIsShowing()
{
    notImplemented();
    return false;
}

void EditorClientHaiku::getGuessesForWord(const String&, Vector<String>&)
{
    notImplemented();
}

void EditorClientHaiku::setInputMethodState(bool enabled)
{
    notImplemented();
}

bool EditorClientHaiku::isEditing() const
{
    return m_editing;
}

// #pragma mark -

bool EditorClientHaiku::handleEditingKeyboardEvent(KeyboardEvent* event,
    const PlatformKeyboardEvent* platformEvent)
{
    Frame* frame = m_page->page()->focusController()->focusedOrMainFrame();
    if (!frame || !frame->document()->focusedNode())
        return false;

    Node* start = frame->selection()->start().node();
    if (!start || !start->isContentEditable())
        return false;

    switch (platformEvent->windowsVirtualKeyCode()) {
    case VK_BACK:
        frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                                             platformEvent->ctrlKey() ? WordGranularity : CharacterGranularity,
                                             false, true);
        break;
    case VK_DELETE:
        frame->editor()->deleteWithDirection(SelectionController::FORWARD,
                                             platformEvent->ctrlKey() ? WordGranularity : CharacterGranularity,
                                             false, true);
        break;
    case VK_LEFT:
        frame->selection()->modify(platformEvent->shiftKey() ? SelectionController::EXTEND : SelectionController::MOVE,
                                   SelectionController::LEFT,
                                   platformEvent->ctrlKey() ? WordGranularity : CharacterGranularity,
                                   true);
        break;
    case VK_RIGHT:
        frame->selection()->modify(platformEvent->shiftKey() ? SelectionController::EXTEND : SelectionController::MOVE,
                                   SelectionController::RIGHT,
                                   platformEvent->ctrlKey() ? WordGranularity : CharacterGranularity,
                                   true);
        break;
    case VK_UP:
        frame->selection()->modify(platformEvent->shiftKey() ? SelectionController::EXTEND : SelectionController::MOVE,
                                   SelectionController::BACKWARD,
                                   platformEvent->ctrlKey() ? ParagraphGranularity : LineGranularity,
                                   true);
        break;
    case VK_DOWN:
        frame->selection()->modify(platformEvent->shiftKey() ? SelectionController::EXTEND : SelectionController::MOVE,
                                   SelectionController::FORWARD,
                                   platformEvent->ctrlKey() ? ParagraphGranularity : LineGranularity,
                                   true);
        break;
    case VK_HOME:
        if (platformEvent->shiftKey() && platformEvent->ctrlKey())
            frame->editor()->command("MoveToBeginningOfDocumentAndModifySelection").execute();
        else if (platformEvent->shiftKey())
            frame->editor()->command("MoveToBeginningOfLineAndModifySelection").execute();
        else if (platformEvent->ctrlKey())
            frame->editor()->command("MoveToBeginningOfDocument").execute();
        else
            frame->editor()->command("MoveToBeginningOfLine").execute();
        break;
    case VK_END:
        if (platformEvent->shiftKey() && platformEvent->ctrlKey())
            frame->editor()->command("MoveToEndOfDocumentAndModifySelection").execute();
        else if (platformEvent->shiftKey())
            frame->editor()->command("MoveToEndOfLineAndModifySelection").execute();
        else if (platformEvent->ctrlKey())
            frame->editor()->command("MoveToEndOfDocument").execute();
        else
            frame->editor()->command("MoveToEndOfLine").execute();
        break;
    case VK_PRIOR:  // PageUp
        if (platformEvent->shiftKey())
            frame->editor()->command("MovePageUpAndModifySelection").execute();
        else
            frame->editor()->command("MovePageUp").execute();
        break;
    case VK_NEXT:  // PageDown
        if (platformEvent->shiftKey())
            frame->editor()->command("MovePageDownAndModifySelection").execute();
        else
            frame->editor()->command("MovePageDown").execute();
        break;
    case VK_RETURN:
        if (platformEvent->shiftKey())
            frame->editor()->command("InsertLineBreak").execute();
        else
            frame->editor()->command("InsertNewline").execute();
        break;
    case VK_TAB:
        return false;
    default:
        if (!platformEvent->ctrlKey() && !platformEvent->altKey() && !platformEvent->text().isEmpty()) {
            if (platformEvent->text().length() == 1) {
                UChar ch = platformEvent->text()[0];
                // Don't insert null or control characters as they can result in unexpected behaviour
                if (ch < ' ')
                    break;
            }
            frame->editor()->insertText(platformEvent->text(), event);
        } else if (platformEvent->ctrlKey()) {
            switch (platformEvent->windowsVirtualKeyCode()) {
            case VK_B:
                frame->editor()->command("ToggleBold").execute();
                break;
            case VK_I:
                frame->editor()->command("ToggleItalic").execute();
                break;
            case VK_V:
                frame->editor()->command("Paste").execute();
                break;
            case VK_X:
                frame->editor()->command("Cut").execute();
                break;
            case VK_Y:
            case VK_Z:
                if (platformEvent->shiftKey())
                    frame->editor()->command("Redo").execute();
                else
                    frame->editor()->command("Undo").execute();
                break;
            default:
                return false;
            }
        } else
            return false;
    }

    return true;
}

void EditorClientHaiku::setPendingComposition(const char* newComposition)
{
    m_pendingComposition = newComposition;
}

void EditorClientHaiku::setPendingPreedit(const char* newPreedit)
{
    m_pendingPreedit = newPreedit;
}

void EditorClientHaiku::clearPendingIMData()
{
    setPendingComposition(0);
    setPendingPreedit(0);
}

void EditorClientHaiku::imContextCommitted(const char* str, EditorClient* client)
{
    // This signal will fire during a keydown event. We want the contents of the
    // field to change right before the keyup event, so we wait until then to actually
    // commit this composition.
    setPendingComposition(str);
}

void EditorClientHaiku::imContextPreeditChanged(EditorClient* client)
{
    const char* newPreedit = 0;
    // TODO: get pre edit
    setPendingPreedit(newPreedit);
}


} // namespace WebCore

