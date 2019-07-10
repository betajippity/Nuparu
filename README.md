Nuparu
======

v0.3.19.16a

Third Party dependencies and foundation libraries for Karl's graphics projects

Nuparu currently consists of:

* [GLM](http://glm.g-truc.net) 0.9.9.3 (Src)
* [Eigen](http://eigen.tuxfamily.org/) 3.3.7 (Src)
* [FreeGLUT](http://freeglut.sourceforge.net) 3.0.0 (Win)
* [GLFW](http://www.glfw.org) 3.3 (Mac/Win/Linux)
* [GLEW](https://github.com/nigels-com/glew) 2.1.0 (Mac/Win/Linux)
* [Embree](https://embree.github.io) 3.5.0 (Mac/Win/Linux)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 1.8.4 (Mac/Win/Linux)
* [OpenEXR](http://www.openexr.com) 2.3.0 (Mac/Win/Linux)
* [stb_image](https://github.com/nothings/stb) 2.20 (Src)
* [Ptex](http://ptex.us) 2.3.1 (Mac/Win/Linux)
* [Partio](https://www.disneyanimation.com/technology/partio.html) 1.7.3 (Mac/Win/Linux)
* [RMSD](http://boscoh.com/code/) (Src)
* [glslUtility](https://github.com/CIS565-Fall-2012/Project0-Cuda-Checker/blob/master/HW0_MAC/src/glslUtility.cpp) 1.2 (Src)
* [Leonhard Gruenschloss's Sobol Generator](http://gruenschloss.org) (Src)
* [NanoGUI](https://github.com/wjakob/nanogui) (f8a4fe) (Mac/Win/Linux)
* [tinyformat](https://github.com/c42f/tinyformat) 2.1.0 (Src)
* [Zlib](https://www.zlib.net) 1.2.11 (Win/Linux)
* [TBB](https://www.threadingbuildingblocks.org/) 2019 Update 5 (Mac/Win/Linux)
* [Boost](http://www.boost.org) 1.69.0 (Mac/Win/Linux)
* [OpenSubdiv](http://graphics.pixar.com/opensubdiv/docs/intro.html)  3.3.3 (Mac/Win/Linux)
* [OpenVDB](http://www.openvdb.org/) 6.0.0 (Mac/Win/Linux)
* [blosc](https://github.com/Blosc) 1.16.2 (Mac/Win/Linux)
* [Protobuf](https://developers.google.com/protocol-buffers/) 3.6.1 (Mac/Win/Linux)
* [ispc](https://ispc.github.io) 1.10.0 (Mac/Win/Linux)
* [dear imgui](https://github.com/ocornut/imgui/releases) 1.71 (Src)

Notes:

* On macOS, all binary libraries except for TBB are statically linked.
* On Windows, all binary libraries except for TBB are statically linked.
* On Linux, all binary libraries except for TBB are statically linked.
* Everything is built using C++14.
* The version of TBB included uses the standard STL, not the Intel Parallel STL implementation.
* Since macOS ships with Zlib and GLUT by default, Nuparu does not include Zlib or FreeGLUT for macOS.
* Nuparu does not contain the entirety of Boost; instead, only a subset of Boost required for OpenVDB is included.
* Partio is built without support for .prt files.
* RMSD has minor modifications for C++ support.
* Embree is built using ispc with support for up to SSE4.2. No AVX support is included.
* OpenSubdiv is built with Ptex and TBB support only. Other backends, such as OpenCL, are not included.
* blosc is built without AVX2 support.
* OpenVDB is built without ABI compatibility for OpenVDB 3.x.
* dear imgui includes the core imgui library and the GLFW3/OpenGL3 implementation files.
* All binary macOS libraries are built on at least macOS Mojave with macOS's Clang variant (Apple LLVM 10.0 or greater) using libc++.
* All binary Windows libraries are built on Windows 10 using /MT with Visual Studio 2017.
* All binary Linux libraries are built using GCC 8.2 or later on at least Fedora 29 and at least Red Hat Enterprise Linux (RHEL) 7.6.
    * On RHEL, GCC 8.2 is used through [Red Hat Developer Toolset 8.0](https://developers.redhat.com/products/developertoolset/updates/).
    * Fedora libraries are in lib/linux, RHEL libraries are in lib/rhel
* All binary libraries on all platforms are built for x86-64.
* Licenses for each library are included either in each library's include/ directory, or in source files directly.
