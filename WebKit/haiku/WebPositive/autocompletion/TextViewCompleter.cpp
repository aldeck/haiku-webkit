/*
 * Copyright 2002-2006, project beam (http://sourceforge.net/projects/beam).
 * All rights reserved. Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Oliver Tappe <beam@hirschkaefer.de>
 */

#include "TextViewCompleter.h"

#include <Looper.h>
#include <TextControl.h>
#include <stdio.h>

#include "AutoCompleterDefaultImpl.h"


// #pragma mark - TextViewWrapper


TextViewCompleter::TextViewWrapper::TextViewWrapper(BTextView* textView)
	:
	fTextView(textView)
{
}


void
TextViewCompleter::TextViewWrapper::GetEditViewState(BString& text,
	int32* caretPos)
{
	if (fTextView && fTextView->LockLooper()) {
		text = fTextView->Text();
		if (caretPos) {
			int32 end;
			fTextView->GetSelection(caretPos, &end);
		}
		fTextView->UnlockLooper();
	}
}


void
TextViewCompleter::TextViewWrapper::SetEditViewState(const BString& text,
	int32 caretPos, int32 selectionLength)
{
	if (fTextView && fTextView->LockLooper()) {
		fTextView->SetText(text.String(), text.Length());
		fTextView->Select(caretPos, caretPos + selectionLength);
		fTextView->ScrollToSelection();
		fTextView->UnlockLooper();
	}
}


BRect
TextViewCompleter::TextViewWrapper::GetAdjustmentFrame()
{
	BRect frame = fTextView->Bounds();
	frame = fTextView->ConvertToScreen(frame);
	frame.InsetBy(0, -3);
	return frame;
}


TextViewCompleter::TextViewCompleter(BTextView* textView, ChoiceModel* model,
		PatternSelector* patternSelector)
	:
	BAutoCompleter(new TextViewWrapper(textView), model,
		new BDefaultChoiceView(), patternSelector),
	BMessageFilter(B_KEY_DOWN),
	fTextView(textView)
{
	fTextView->AddFilter(this);
}


TextViewCompleter::~TextViewCompleter()
{
	fTextView->RemoveFilter(this);
}


filter_result
TextViewCompleter::Filter(BMessage* message, BHandler** target)
{
	const char* bytes;
	int32 modifiers;
	if (!target || message->FindString("bytes", &bytes) != B_OK
		|| message->FindInt32("modifiers", &modifiers) != B_OK) {
		return B_DISPATCH_MESSAGE;
	}

	switch (bytes[0]) {
		case B_UP_ARROW:
			SelectPrevious();
			return B_SKIP_MESSAGE;
		case B_DOWN_ARROW:
			SelectNext();
			return B_SKIP_MESSAGE;
		case B_ESCAPE:
			CancelChoice();
			return B_SKIP_MESSAGE;
		case B_RETURN:
			if (IsChoiceSelected()) {
				ApplyChoice();
				EditViewStateChanged();
			} else
				CancelChoice();
			return B_DISPATCH_MESSAGE;
		case B_TAB: {
			// make sure that the choices-view is closed when tabbing out:
			CancelChoice();
			return B_DISPATCH_MESSAGE;
		}
		default:
			// dispatch message to textview manually...
			Looper()->DispatchMessage(message, *target);
			// ...and propagate the new state to the auto-completer:
			EditViewStateChanged();
			return B_SKIP_MESSAGE;
	}
	return B_DISPATCH_MESSAGE;
}
