/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef InlineASM_h
#define InlineASM_h

#include <wtf/Platform.h>

/* asm directive helpers */ 

#if OS(DARWIN) || (OS(WINDOWS) && CPU(X86))
#define SYMBOL_STRING(name) "_" #name
#else
#define SYMBOL_STRING(name) #name
#endif

#if OS(IOS)
#define THUMB_FUNC_PARAM(name) SYMBOL_STRING(name)
#else
#define THUMB_FUNC_PARAM(name)
#endif

#if (OS(LINUX) || OS(FREEBSD) || OS(HAIKU)) && CPU(X86_64)
#define SYMBOL_STRING_RELOCATION(name) #name "@plt"
#elif OS(DARWIN) || (CPU(X86_64) && COMPILER(MINGW) && !GCC_VERSION_AT_LEAST(4, 5, 0))
#define SYMBOL_STRING_RELOCATION(name) "_" #name
#elif CPU(X86) && COMPILER(MINGW)
#define SYMBOL_STRING_RELOCATION(name) "@" #name "@4"
#else
#define SYMBOL_STRING_RELOCATION(name) SYMBOL_STRING(name)
#endif

#if OS(DARWIN)
    // Mach-O platform
#define HIDE_SYMBOL(name) ".private_extern _" #name
#elif OS(AIX)
    // IBM's own file format
#define HIDE_SYMBOL(name) ".lglobl " #name
#elif   OS(LINUX)               \
     || OS(FREEBSD)             \
     || OS(OPENBSD)             \
     || OS(SOLARIS)             \
     || (OS(HPUX) && CPU(IA64)) \
     || OS(NETBSD)
    // ELF platform
#define HIDE_SYMBOL(name) ".hidden " #name
#else
#define HIDE_SYMBOL(name)
#endif

// FIXME: figure out how this works on all the platforms. I know that
// on Linux, the preferred form is ".Lstuff" as opposed to "Lstuff".
// Don't know about any of the others.
#if PLATFORM(MAC)
#define LOCAL_LABEL_STRING(name) "L" #name
#endif

#endif // InlineASM_h
