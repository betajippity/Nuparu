Nuparu
======

v0.8.24.18

Third Party dependencies and foundation libraries for Karl's graphics projects

Nuparu currently consists of:

* [GLM](http://glm.g-truc.net) 1.0.1 (Src)
* [FreeGLUT](http://freeglut.sourceforge.net) 3.4.0 (Win)
* [GLFW](http://www.glfw.org) 3.3.8 (Mac/Win/Linux)
* [GLEW](https://github.com/nigels-com/glew) 2.2.0 (Mac/Win/Linux)
* [Embree](https://embree.github.io) 4.3.0 (Mac/Win/Linux)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 1.9.5 (Mac/Win/Linux)
* [Imath](https://github.com/AcademySoftwareFoundation/Imath) 3.1.9 (Mac/Win/Linux)
* [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr) 3.2.1 (Mac/Win/Linux)
* [stb_image](https://github.com/nothings/stb) 2.28 (Src)
* [Ptex](http://ptex.us) 2.4.2 (Mac/Win/Linux)
* [Partio](https://www.disneyanimation.com/technology/partio.html) 1.17.1 (Mac/Win/Linux)
* [RMSD](http://boscoh.com/code/) (Src)
* [Leonhard Gruenschloss's Sobol Generator](http://gruenschloss.org) (Src)
* [tinyformat](https://github.com/c42f/tinyformat) 2.3.0 (Src)
* [Zlib](https://www.zlib.net) 1.3.1 (Mac/Win/Linux)
* [TBB](https://www.threadingbuildingblocks.org/) 2021.11.0 (Mac/Win/Linux)
* [Boost](http://www.boost.org) 1.84.0 (Mac/Win/Linux)
* [OpenSubdiv](http://graphics.pixar.com/opensubdiv/docs/intro.html) 3.6.0 (Mac/Win/Linux)
* [OpenVDB](http://www.openvdb.org/) 11.0.0 (Mac/Win/Linux)
* [NanoVDB](http://www.openvdb.org/) 11.0.0 (Src)
* [blosc](https://github.com/Blosc) 1.21.5 (Mac/Win/Linux)
* [cereal](https://uscilab.github.io/cereal/) 1.3.2 (Src)
* [ispc](https://ispc.github.io) 1.18.0 (Mac/Win/Linux)
* [dear imgui](https://github.com/ocornut/imgui) 1.90.3 with Docking (Src)
* [imnodes](https://github.com/Nelarius/imnodes) 0.5 (Src)
* [cpuinfo](https://github.com/pytorch/cpuinfo) f42f5ea (Mac/Win/Linux)
* [zmq](https://github.com/zeromq/libzmq) 4.3.5 (Mac/Win/Linux)

Notes:

* On all platforms, all binary libraries except for TBB are statically linked.
* x86-64 builds are included for macOS, Windows, and RHEL 9.x (Linux-GCC12), and Fedora 39 (Linux-GCC13).
* arm64 builds are included for macOS, Fedora 39 (Linux-ARM), and NVIDIA L4T 32.6.1 (Linux-L4T, which is based on and should be largely compatible with Ubuntu 18.04).
* macOS builds are provided as Universal Binaries incorporating arm64 and x86-64 versions into single libraries instead of separate library files per architecture.
* Everything is built using C++14 or newer.
* The version of TBB included uses the standard STL, not the Intel Parallel STL implementation.
* Since macOS ships with GLUT by default, Nuparu does not include FreeGLUT for macOS.
* Nuparu does not contain the entirety of Boost; instead, only a subset of Boost required for OpenVDB is included.
* Partio is built without support for .prt files.
* RMSD has minor modifications for C++ support.
* On x86-64, Embree is built using ispc with support for up to SSE4.2. No AVX support is included. On arm64, Embree is built with NEON 2X support.
* Embree is built with a max nested instancing depth of 16.
* Embree currently is built without SYCL support; this will be added later.
* OpenEXR is built without version numbers in namespaces.
* OpenSubdiv is built with Ptex, TBB, and OpenGL support on all platforms. Additionally:
    * CUDA is supported on Windows, Fedora 39 x86-64, L4T, and RHEL 9.x
    * Metal is supported on macOS
    * OpenSubdiv has been modified to work with OneTBB
* blosc is built without AVX2 support.
* OpenVDB is built without ABI compatibility for OpenVDB 3.x.x and without explicit instantiation.
* NanoVDB is built with CUDA support.
* dear imgui includes the core imgui library and the GLFW3/OpenGL3 and AppKit/Metal implementation files.
* Only a subset of boost necessary to support OpenVDB and USD is included.
* For boost, the following versions of Python are supported:
    * On macOS, Python 3.11 from MacPorts
    * On Fedora 39, the default system Python 3.12
    * On RHEL, the default system Python 3.9
    * On L4T, the default system Python 3.6
    * Python support is not provided on Windows
* ZMQ is built without Websockets and without TLS support
* macOS libraries require at minimum macOS Sonoma and macOS's Clang variant (Apple LLVM 15 or greater) with libc++.
* Windows libraries require at minimum Windows 11 and Visual Studio 2022 with \mt.
* Linux libraries require at minimum either Fedora 39, Red Hat Enterprise Linux (RHEL) 9.x, or NVIDIA L4T 32.6.1.
    * RHEL 9.x libraries require GCC 12.x on x86-64 and are in lib/linux-gcc12
    * Fedora 39 libraries require GCC 13.x on x86-64 and are in lib/linux-gcc13
    * Fedora 39 libraries require GCC 13.x on ARM arm64 and are in lib/linux-arm
    * L4T libraries require GCC 7.5.x and are in lib/linux-l4t
* Things with CUDA support are built using CUDA 12.1, except on L4T, in which case CUDA 10.2 is used
* Licenses for each library are included either in each library's include/ directory, or in source files directly.
