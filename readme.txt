Project Hosted on:
pal.sourceforge.net

Directory structure:
framework/
  - contains various base functinality, such as the pluggable factory and memory manager (stripped down)
pal/
  - contains the abstract PAL headers
pal_i/
  - contains various PAL implementations
sdlgl/
  - contains files for basic visualization
extras/
  - contains extra utilities, such as the code for the configuration tool

The configuration utility will automatically create:
bin/
 - contains the binaries
lib/
 - contains the libraries

Each will have a sub directory for your compiler and for "release" and "debug" builds, if you set the BUILD_OUTPUT_DIR_RELEASE_DEBUG CMake option to ON.

----------------------------------------------------------------------------------------------------------
Building on Mac OS X
----------------------------------------------------------------------------------------------------------
After you run Cmake, There are some changes which need to be manually performed before it will
compile on Mac OS X.

Arch: i386
User Header Search Paths: 
/lib/PAL/pal
/lib/pal
/lib/irrlicht-1.6/include/
Libraries  (add new group, then add):
libIrrlicht.a
Frameworks  (add new group, then add):
SDL.framework
OpenGL.framework
Carbon.framework
Cocoa.framework
CoreFoundation.framework
IOKit.framework

Targets that need SDLMain.h and SDLMain.m located in MacOSX subfolders:
palDemo
test_drop

palBenchmark:
1) Add > New Build Phase > New Link Binary with Libraries Build Phase
2) Then drag the Frameworks that it needs to the build phase:
	- CoreFoundation.framework
	- Carbon.framework
	- Cocoa.framework
	- OpenGL.framework
	- libIrrlicht.a
	- IOKit.framework
palDemo, test_drop:
1) Right click > Add Existing Files > palBenchmark/MacOSX (because we need the SDLMain)
2) Drag SDLMain.h and SDLMain.h to the "Compile Sources" build stage.

----------------------------------------------------------------------------------------------------------