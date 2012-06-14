/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "config.h"
#include "TextBreakIteratorInternalICU.h"

#include <Locale.h>
#include <LocaleRoster.h>

namespace WebCore {

static const char* currentLocaleID()
{
    static BString local;
    static bool initialized = false;
    if (!initialized) {
    	initialized = true;
    	BLanguage* language;
        if (be_locale_roster->GetDefaultLanguage(&language) == B_OK)
            local = language->Code();
        else
            local = "en_US";
    }
    return local.String();
}

const char* currentSearchLocaleID()
{
    return currentLocaleID();
}

const char* currentTextBreakLocaleID()
{
    return currentLocaleID();
}

} // namespace WebCore

