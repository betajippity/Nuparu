Nuparu
======

v0.7.23.11

Third Party dependencies and foundation libraries for Karl's graphics projects

Nuparu currently consists of:

* [GLM](http://glm.g-truc.net) 0.9.9.8 (Src)
* [FreeGLUT](http://freeglut.sourceforge.net) 3.4.0 (Win)
* [GLFW](http://www.glfw.org) 3.3.8 (Mac/Win/Linux)
* [GLEW](https://github.com/nigels-com/glew) 2.2.0 (Mac/Win/Linux)
* [Embree](https://embree.github.io) 4.0.1 (Mac/Win/Linux)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 1.9.5 (Mac/Win/Linux)
* [Imath](https://github.com/AcademySoftwareFoundation/Imath) 3.1.7 (Mac/Win/Linux)
* [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr) 3.1.6 (Mac/Win/Linux)
* [stb_image](https://github.com/nothings/stb) 2.28 (Src)
* [Ptex](http://ptex.us) 2.4.2 (Mac/Win/Linux)
* [Partio](https://www.disneyanimation.com/technology/partio.html) 1.14.6 (Mac/Win/Linux)
* [RMSD](http://boscoh.com/code/) (Src)
* [Leonhard Gruenschloss's Sobol Generator](http://gruenschloss.org) (Src)
* [tinyformat](https://github.com/c42f/tinyformat) 2.3.0 (Src)
* [Zlib](https://www.zlib.net) 1.2.13 (Mac/Win/Linux)
* [TBB](https://www.threadingbuildingblocks.org/) 2021.8.0 (Mac/Win/Linux)
* [Boost](http://www.boost.org) 1.81.0 (Mac/Win/Linux)
* [OpenSubdiv](http://graphics.pixar.com/opensubdiv/docs/intro.html) 3.5.0 (Mac/Win/Linux)
* [OpenVDB](http://www.openvdb.org/) 10.0.1 (Mac/Win/Linux)
* [NanoVDB](http://www.openvdb.org/) 10.0.1 (Src)
* [blosc](https://github.com/Blosc) 1.21.3 (Mac/Win/Linux)
* [cereal](https://uscilab.github.io/cereal/) 1.3.2 (Src)
* [ispc](https://ispc.github.io) 1.18.0 (Mac/Win/Linux)
* [dear imgui](https://github.com/ocornut/imgui) 1.89.4 with Docking (Src)
* [imnodes](https://github.com/Nelarius/imnodes) 0.5 (Src)
* [im-neo-sequencer](https://gitlab.com/GroGy/im-neo-sequencer) 0.1 (Src)

Notes:

* On all platforms, all binary libraries except for TBB are statically linked.
* x86-64 builds are included for macOS, Windows, and RHEL 7.9 (Linux-GCC10), and Fedora 37 (Linux-GCC12).
* arm64 builds are included for macOS, Fedora 37 (Linux-ARM), and NVIDIA L4T 32.6.1 (Linux-L4T, which is based on and should be largely compatible with Ubuntu 18.04).
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
    * CUDA is supported on Windows, Fedora 37 x86-64, L4T, and RHEL
    * Metal is supported on macOS
* blosc is built without AVX2 support.
* OpenVDB is built without ABI compatibility for OpenVDB 3.x.x and without explicit instantiation.
* NanoVDB is built with CUDA support.
* dear imgui includes the core imgui library and the GLFW3/OpenGL3 and AppKit/Metal implementation files.
* im-neo-sequencer includes some bugfixes and additional functionality not found in the original project
* Only a subset of boost necessary to support OpenVDB and USD is included.
* For boost, the following versions of Python are supported:
    * On macOS, Python 3.11 from MacPorts
    * On Fedora 37, the default system Python 3.11
    * On RHEL, the default system Python 3.6
    * On L4T, the default system Python 3.6
    * Python support is not provided on Windows
* macOS libraries require at minimum macOS Ventura and macOS's Clang variant (Apple LLVM 14 or greater) with libc++.
* Windows libraries require at minimum Windows 11 and Visual Studio 2022 with \mt.
* Linux libraries require at minimum either Fedora 37, Red Hat Enterprise Linux (RHEL) 7.9, or NVIDIA L4T 32.6.1.
    * On RHEL, GCC 10.2.1 is used through [Red Hat Developer Toolset 10.0](https://developers.redhat.com/products/developertoolset/updates/).
    * Fedora 37 libraries require GCC 12.x on x86-64 and are in lib/linux-gcc12
    * Fedora 37 libraries require GCC 12.x on ARM arm64 and are in lib/linux-arm
    * RHEL libraries require GCC 10.2.x and are in lib/linux-gcc10
    * L4T libraries require GCC 7.5.x and are in lib/linux-l4t
* Things with CUDA support are built using CUDA 12.1, except on L4T, in which case CUDA 10.2 is used
* Licenses for each library are included either in each library's include/ directory, or in source files directly.
