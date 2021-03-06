Nuparu
======

v0.4.21.20a

Third Party dependencies and foundation libraries for Karl's graphics projects

Nuparu currently consists of:

* [GLM](http://glm.g-truc.net) 0.9.9.8 (Src)
* [FreeGLUT](http://freeglut.sourceforge.net) 3.2.1 (Win)
* [GLFW](http://www.glfw.org) 3.3.4 (Mac/Win/Linux)
* [GLEW](https://github.com/nigels-com/glew) 2.2.0 (Mac/Win/Linux)
* [Embree](https://embree.github.io) 3.13.0 (Mac/Win/Linux)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 1.9.4 (Mac/Win/Linux)
* [Imath](https://github.com/AcademySoftwareFoundation/Imath) 3.0.3 (Mac/Win/Linux)
* [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr) 3.0.3 (Mac/Win/Linux)
* [stb_image](https://github.com/nothings/stb) 2.26 (Src)
* [Ptex](http://ptex.us) 2.4.0 (Mac/Win/Linux)
* [Partio](https://www.disneyanimation.com/technology/partio.html) 1.14.0 (Mac/Win/Linux)
* [RMSD](http://boscoh.com/code/) (Src)
* [Leonhard Gruenschloss's Sobol Generator](http://gruenschloss.org) (Src)
* [tinyformat](https://github.com/c42f/tinyformat) 2.3.0 (Src)
* [Zlib](https://www.zlib.net) 1.2.11 (Mac/Win/Linux)
* [TBB](https://www.threadingbuildingblocks.org/) 2020.3 (Mac/Win/Linux)
* [Boost](http://www.boost.org) 1.76.0 (Mac/Win/Linux)
* [OpenSubdiv](http://graphics.pixar.com/opensubdiv/docs/intro.html) 3.4.4 (Mac/Win/Linux)
* [OpenVDB](http://www.openvdb.org/) 8.0.1 (Mac/Win/Linux)
* [blosc](https://github.com/Blosc) 1.21.0 (Mac/Win/Linux)
* [Protobuf](https://developers.google.com/protocol-buffers/) 3.17.0 (Mac/Win/Linux)
* [ispc](https://ispc.github.io) 1.15 (Mac/Win/Linux)
* [dear imgui](https://github.com/ocornut/imgui/releases) 1.82 (Src)
* [imnodes](https://github.com/Nelarius/imnodes) 0.4 (Src)

Notes:

* On all platforms, all binary libraries except for TBB are statically linked.
* x86-64 builds are included for macOS, Windows, and Linux-GCC8/Linux-GCC10.
* arm64 builds are included for macOS and Linux-GCC10.
* macOS builds are provided as Universal Binaries incorporating arm64 and x86-64 versions into single libraries instead of separate library files per architecture.
* Everything is built using C++14 or newer.
* The version of TBB included uses the standard STL, not the Intel Parallel STL implementation.
* Since macOS ships with GLUT by default, Nuparu does not include FreeGLUT for macOS.
* Nuparu does not contain the entirety of Boost; instead, only a subset of Boost required for OpenVDB is included.
* Partio is built without support for .prt files.
* RMSD has minor modifications for C++ support.
* On x86-64, Embree is built using ispc with support for up to SSE4.2. No AVX support is included. On arm64, Embree is built with NEON support.
* Embree is built with a max nested instancing depth of 8.
* OpenEXR is built without version numbers in namespaces.
* OpenSubdiv is built with Ptex and TBB support only. Other backends, such as OpenCL, are not included.
* blosc is built without AVX2 support.
* OpenVDB is built without ABI compatibility for OpenVDB 3.x.
* dear imgui includes the core imgui library and the GLFW3/OpenGL3 implementation files.
* macOS libraries require at minimum macOS Big Sur and macOS's Clang variant (Apple LLVM 11.0 or greater) with libc++.
* Windows libraries require at minimum Windows 10 and Visual Studio 2019 with \mt. 
* Linux libraries require at minimum either Fedora 33 or Red Hat Enterprise Linux (RHEL) 7.6.
    * On RHEL, GCC 8.3 is used through [Red Hat Developer Toolset 8.0](https://developers.redhat.com/products/developertoolset/updates/).
    * Fedora 33 libraries require GCC 10.1 on x86-64 and are in lib/linux-gcc10
    * Fedora 33 libraries require GCC 10.1 on ARM arm64 and are in lib/linux-arm
    * RHEL libraries require GCC 8.3 and are in lib/linux-gcc8
* Licenses for each library are included either in each library's include/ directory, or in source files directly.
