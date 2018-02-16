Nuparu
======

v0.1.17.32a

Third Party dependencies and foundation libraries for Karl's graphics projects

Nuparu currently consists of:

* [OpenVDB](http://www.openvdb.org/) 2.3.0 (OSX/Win)

Updated:

* [GLM](http://glm.g-truc.net) 0.9.8.5 (Src)
* [Eigen](http://eigen.tuxfamily.org/) 3.3.4 (Src)
* [FreeGLUT](http://freeglut.sourceforge.net) 3.0.0 (Win)
* [GLFW](http://www.glfw.org) 3.2.1 (Mac/Win)
* [GLEW](https://github.com/nigels-com/glew) 2.1.0 (Mac/Win)
* [Embree](https://embree.github.io) 2.17.2 (Mac/Win)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 1.8.4 (Mac/Win)
* [OpenEXR](http://www.openexr.com) 2.2.1 (Mac/Win)
* [stb_image](https://github.com/nothings/stb) 2.19 (Src)
* [Ptex](http://ptex.us) 2.1.33 (Mac/Win)
* [Partio](https://www.disneyanimation.com/technology/partio.html) 1.1.0 (8b6ea0d) (Mac/Win)
* [RMSD](http://boscoh.com/code/) (Src)
* [glslUtility](https://github.com/CIS565-Fall-2012/Project0-Cuda-Checker/blob/master/HW0_MAC/src/glslUtility.cpp) 1.2 (Src)
* [Leonhard Gruenschloss's Sobol Generator](http://gruenschloss.org) (Src)
* [NanoGUI](https://github.com/wjakob/nanogui) (cf576ee) (Mac/Win)
* [tinyformat](https://github.com/c42f/tinyformat) 2.1.0 (Src)
* [Zlib](https://www.zlib.net) 1.2.11 (Win)
* [TBB](https://www.threadingbuildingblocks.org/) 2018 Update 2 (Mac/Win)
* [Boost](http://www.boost.org) 1.66.0 (Src)
* [OpenSubdiv](http://graphics.pixar.com/opensubdiv/docs/intro.html)  3.3.1 (Mac/Win)

Notes:

* On macOS, all binary libraries except for TBB are statically linked
* On Windows, all binary libraries except for TBB are statically linked
* Since macOS ships with Zlib and GLUT by default, Nuparu does not include Zlib or FreeGLUT for macOS
* Nuparu does not contain the entirety of Boost; instead, only a subset of Boost required for OpenVDB is included. The components of Boost included are header-only.
* OpenVDB has a slight modification in Coord.h for Windows support
* Partio on Windows does not have support for .prt files
* RMSD has minor modifications for C++ support
* OpenSubdiv is built with Ptex and TBB support only. Other backends, such as OpenCL, are not included.
* All binary macOS libraries are built with macOS's Clang variant (Apple LLVM 5.1 or greater) using libc++.
* All binary Windows libraries are built using /MT with Visual Studio 2017.
* All binary libraries on all platforms are built for x86-64
* Licenses for each library are included either in each library's include/ directory, or in source files directly
