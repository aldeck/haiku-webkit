/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2007 Ryan Leavengood <leavengood@gmail.com>
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

#ifndef ClipboardHaiku_h
#define ClipboardHaiku_h

#include "Clipboard.h"
#include "ClipboardAccessPolicy.h"

namespace WebCore {

class CachedImage;

// State available during IE's events for drag and drop and copy/paste.
class ClipboardHaiku : public Clipboard {
public:
    static PassRefPtr<ClipboardHaiku> create(ClipboardAccessPolicy policy, const DragData* dragData, Frame* frame)
    {
        return adoptRef(new ClipboardHaiku(policy, dragData, frame));
    }
    static PassRefPtr<ClipboardHaiku> create(ClipboardAccessPolicy policy, Frame* frame, ClipboardType clipboardType = CopyAndPaste)
    {
        return adoptRef(new ClipboardHaiku(policy, clipboardType, frame));
    }
    virtual ~ClipboardHaiku() {};

    virtual void clearData(const String& type);
    virtual void clearAllData();
    virtual String getData(const String& type) const;
    virtual bool setData(const String& type, const String& data);

    // Extensions beyond IE's API.
    virtual HashSet<String> types() const;
    virtual PassRefPtr<FileList> files() const;

    IntPoint dragLocation() const;
    CachedImage* dragImage() const;
    virtual void setDragImage(CachedImage*, const IntPoint&);
    Node* dragImageElement();
    virtual void setDragImageElement(Node*, const IntPoint&);

    virtual DragImageRef createDragImage(IntPoint& dragLocation) const;
    virtual void declareAndWriteDragImage(Element*, const KURL&, const String& title, Frame*);
    virtual void writeURL(const KURL&, const String&, Frame*);
    virtual void writeRange(Range*, Frame*);
    virtual void writePlainText(const String&);

    virtual bool hasData();

private:
    ClipboardHaiku(ClipboardAccessPolicy, const DragData* dragData, Frame*);
    ClipboardHaiku(ClipboardAccessPolicy, ClipboardType, Frame*);

    Frame* m_frame;
};

} // namespace WebCore

#endif // ClipboardHaiku_h

