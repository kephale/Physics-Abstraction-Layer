Projcet Hosted on:
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