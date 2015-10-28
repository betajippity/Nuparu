Nuparu
======

v0.1.15.44

Third Party dependencies and foundation libraries for Karl's graphics projects

Nuparu currently consists of:

* [GLM](http://glm.g-truc.net/0.9.4/index.html) 0.9.5.3 (OSX/Linux/Win)
* [GLFW](http://www.glfw.org/) 3.1.1 (OSX/Linux/Win)
* [GLEW](http://glew.sourceforge.net/index.html) 1.10.0 (OSX/Linux/Win)
* [jsoncpp](http://sourceforge.net/projects/jsoncpp/) 0.10.5 (OSX/Win)
* [stb_image](https://code.google.com/p/stblib/) 1.33 (OSX/Linux/Win)
* [glslUtility](https://github.com/CIS565-Fall-2012/Project0-Cuda-Checker/blob/master/HW0_MAC/src/glslUtility.cpp) 1.2 (OSX/Linux/Win)
* [Eigen](eigen.tuxfamily.org/) 3.2.0 (OSX/Linux/Win)
* [OpenVDB](http://www.openvdb.org/) 2.3.0 (OSX/Win)
* [Partio](http://www.disneyanimation.com/technology/partio.html) 1.1.0 (OSX/Win)
* [RMSD](http://boscoh.com/code/) (OSX/Linux/Win)
* [OpenEXR](http://www.openexr.com) 2.1.0 (OSX/Win)
* [TBB](https://www.threadingbuildingblocks.org/) 4.4 (OSX/Win)
* [Zlib](http://www.zlib.net/) 1.2.8 (Win)
* [Boost](www.boost.org) 1.55.0 (OSX/Linux/Win)
* [Embree](https://embree.github.io) 2.7 (OSX/Win)
* [Leonhard Gruenschloss's Sobol Generator](http://gruenschloss.org) (OSX/Linux/Win)

Notes:

* On OSX, all binary libraries except for TBB are statically linked
* On Windows, all binary libraries except for TBB are statically linked
* Since OSX ships with Zlib by default, Nuparu does not include Zlib for OSX
* Nuparu does not contain the entirety of Boost; instead, only a subset of Boost required for OpenVDB is included. The components of Boost included are header-only.
* OpenVDB has a slight modification in Coord.h for Windows support
* RMSD has minor modifications for C++ support
* All binary OSX libraries are built with OSX's Clang variant (Apple LLVM 5.1 or greater) using libc++.
* All binary Windows libraries are built using /MT with Visual Studio 2013, except for OpenVDB, which is build using /MT with Visual Studio 2012. OpenVDB will be updated soon.
* All binary libraries on all platforms are built for x86-64
* Licenses for each library are included in each library's include/ directory
