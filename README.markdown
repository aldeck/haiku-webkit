# Haiku WebKit #

This repository contains the Haiku WebKit port and WebPositive browser source code.
For more information, please visit the [project's wiki and issue tracker](http://webpositive.haiku-os.org/)

## Quick build instructions ##

### Requirements ###

- You need a recent version of Haiku with the GCC 4 development tools
- The following dependencies: `GPerf, ICU, ICU-devel, Perl, Python`
- And a fast computer!

Dependencies can be installed via:

    $ installoptionalpackage GPerf ICU ICU-devel Curl Perl Python

Or, if you build Haiku from source you can add the packages to your UserBuildConfig:

    AddOptionalHaikuImagePackages GPerf ICU ICU-devel Curl Perl Python ;

Those packages can also be manually downloaded and installed from http://haiku-files.org/files/optional-packages/


### Building WebKit and WebPositive ###

    $ setgcc gcc4
    $ cd Source/JavaScriptCore
    $ ./make-generated-sources.sh
    $ cd Source/WebCore
    $ ./make-generated-sources.sh
    $ NDEBUG=1 jam -qj4 WebPositive

This will build a release version of WebKit libraries and WebPositive on a quad core cpu. Remove `NDEBUG=1` to build a debug version.

On a successful build, executables and libraries are installed in `/boot/home/config/bin` and `/boot/home/config/lib` respectively.


### Advanced Build, other jam targets ###

The following jam targets are available:

- libwtf.so - The Web Template Library
- libjavascriptcore.so -  The JavaScriptCore library
- jsc	 - The JavaScriptCore executable shell
- libwebcore.so - The WebCore library
- libwebkit.so - The WebKit library
- HaikuLauncher - A simple browsing test app
- DumpRenderTree - The tree parsing test tool
- WebPositive - The Haiku browser

Example given, this will build the JavaScriptCore library in debug mode:

    $ jam -q libjavascriptcore.so

To rebuild from scratch a target and all it's dependencies, use the `-a` option:

    $ NDEBUG=1 jam -aqj4 WebPositive

In some rare cases the build system can be confused, to be sure that everything gets rebuilt from scratch,
you can remove the generated/ directory.


## Notes ##

If you switch between Release and Debug build versions, make sure to always
have matching libs in /boot/home/config/lib. For example, if you switch to
release build, do something which updates libwebcore.so, it will be placed
in /boot/home/config/lib, along with libwebkit.so and WebPositive, since
they depend on it. But it won't update libjavascriptcore.so, since it didn't
change. But you need the matching version. As long as our build system doesn't
take care of that, you need to make sure of this manually.


This document was last updated June 18, 2012

Authors: Maxime Simon, Alexandre Deckner
