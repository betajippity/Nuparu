Nuparu
======

v0.3.20.15f-ARM

Third Party dependencies and foundation libraries for Karl's graphics projects

Nuparu currently consists of:

* [GLM](http://glm.g-truc.net) 0.9.9.7 (Src)
* [FreeGLUT](http://freeglut.sourceforge.net) 3.2.1 (Win)
* [GLFW](http://www.glfw.org) 3.3.2 (Mac/Win/Linux)
* [GLEW](https://github.com/nigels-com/glew) 2.1.0 (Mac/Win/Linux)
* [Embree](https://embree.github.io) 3.9.0 (Mac/Win/Linux)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 1.9.2 (Mac/Win/Linux)
* [OpenEXR](http://www.openexr.com) 2.4.1 (Mac/Win/Linux)
* [stb_image](https://github.com/nothings/stb) 2.23 (Src)
* [Ptex](http://ptex.us) 2.3.2 (Mac/Win/Linux)
* [Partio](https://www.disneyanimation.com/technology/partio.html) 1.10.1 (Mac/Win/Linux)
* [RMSD](http://boscoh.com/code/) (Src)
* [Leonhard Gruenschloss's Sobol Generator](http://gruenschloss.org) (Src)
* [tinyformat](https://github.com/c42f/tinyformat) 2.3.0 (Src)
* [Zlib](https://www.zlib.net) 1.2.11 (Mac/Win/Linux)
* [TBB](https://www.threadingbuildingblocks.org/) 2020.2 (Mac/Win/Linux)
* [Boost](http://www.boost.org) 1.72.0 (Mac/Win/Linux)
* [OpenSubdiv](http://graphics.pixar.com/opensubdiv/docs/intro.html)  3.4.3 (Mac/Win/Linux)
* [OpenVDB](http://www.openvdb.org/) 7.0.0 (Mac/Win/Linux)
* [blosc](https://github.com/Blosc) 1.18.1 (Mac/Win/Linux)
* [Protobuf](https://developers.google.com/protocol-buffers/) 3.11.2 (Mac/Win/Linux)
* [ispc](https://ispc.github.io) 1.12.0 (Mac/Win/Linux)
* [dear imgui](https://github.com/ocornut/imgui/releases) 1.75 (Src)
* [imnodes](https://github.com/Nelarius/imnodes) 0.2 (Src)

Notes:

* On macOS, all binary libraries except for TBB are statically linked.
* On Windows, all binary libraries except for TBB are statically linked.
* On Linux, all binary libraries except for TBB are statically linked.
* x86-64 builds are included for macOS, Windows, and Linux-GCC8/Linux-GCC9
* ARM aarch64 builds are included for Linux-GCC9
* Everything is built using C++14.
* The version of TBB included uses the standard STL, not the Intel Parallel STL implementation.
* Since macOS ships with GLUT by default, Nuparu does not include FreeGLUT for macOS.
* Nuparu does not contain the entirety of Boost; instead, only a subset of Boost required for OpenVDB is included.
* Partio is built without support for .prt files.
* RMSD has minor modifications for C++ support.
* On x86-64, Embree is built using ispc with support for up to SSE4.2. No AVX support is included.
* On ARM aarch64, Embree 3.8 is currently included instead of Embree 3.9. The ARM aarch64 build of Embree is based on the [embree-aarch64](https://github.com/lighttransport/embree-aarch64) fork of Embree.
* Embree is built with a max nested instancing depth of 8.
* OpenEXR is built without version numbers in namespaces.
* OpenSubdiv is built with Ptex and TBB support only. Other backends, such as OpenCL, are not included.
* blosc is built without AVX2 support.
* OpenVDB is built without ABI compatibility for OpenVDB 3.x.
* dear imgui includes the core imgui library and the GLFW3/OpenGL3 implementation files.
* All binary macOS libraries are built on at least macOS Catalina with macOS's Clang variant (Apple LLVM 11.0 or greater) using libc++.
* All binary Windows libraries are built on Windows 10 using /MT with Visual Studio 2019.
* All binary Linux libraries are built using GCC 9.2 or later on at least Fedora 31 and at least Red Hat Enterprise Linux (RHEL) 7.6.
    * On RHEL, GCC 8.3 is used through [Red Hat Developer Toolset 8.0](https://developers.redhat.com/products/developertoolset/updates/).
    * Fedora libraries build using GCC 9.2 on x86-64 are in lib/linux-gcc9
    * Fedora libraries build using GCC 9.2 on ARM aarch64 are in lib/linux-arm
    * RHEL libraries build using GCC 8.3 are in lib/linux-gcc8
* Licenses for each library are included either in each library's include/ directory, or in source files directly.
