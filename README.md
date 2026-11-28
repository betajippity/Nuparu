# Nuparu

Nuparu builds and assembles the third-party dependencies used by Karl's
graphics projects. Dependency revisions are pinned by Git submodules in
`deps/`; the build scripts, package assembly and local patches are in
`CMakeLists.txt` and `cmake/`. RMSD and Sobol remain as source files in this
repository. Built packages and intermediate files are local artifacts and are
not checked in. The top-level `src/` and `include/` contain only RMSD and Sobol;
the remaining sources and headers are supplied by the pinned dependencies.

The package uses shared oneTBB and static libraries for the other compiled
dependencies. Windows libraries use the static MSVC runtime. Packages are
specific to their OS, architecture, compiler, configuration and GPU options.

## macOS and Linux

From the Nuparu checkout, choose a distinct name for the host and compiler:

```sh
git submodule update --init --recursive
nuparu_repo="$PWD"
nuparu_arch=full-native-linux-gcc16
cmake -S . -B "build/$nuparu_arch" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$nuparu_repo/packages/$nuparu_arch" \
  -DNUPARU_BUILD_ALL=ON \
  -DNUPARU_OPENSUBDIV_GPU=OFF
cmake --build "build/$nuparu_arch" -j 4
ctest --test-dir "build/$nuparu_arch" --output-on-failure
```

This example builds CPU-only OpenSubdiv. Omit the GPU switch to use its
platform defaults, or choose OpenGL, Metal, and CUDA explicitly.
For native macOS builds, add `-DCMAKE_OSX_ARCHITECTURES=arm64` (or `x86_64`)
and a suitable `-DCMAKE_OSX_DEPLOYMENT_TARGET`. Universal Embree packages
are not supported. Linux requires the GLFW/X11 and OpenGL development packages.
ISPC and the GLEW release archive are downloaded and verified.

## Windows

Use the corresponding x64 or ARM64 native Visual Studio developer prompt.
ARM64 additionally needs the Visual Studio clang-cl tools for Embree; its
x64 ISPC executable runs under emulation. The example below selects CUDA
13.5 and GPU architecture 75; adjust these for your toolkit and GPU. To
build without the OpenSubdiv GPU backends, replace the GPU and CUDA options
with `-DNUPARU_OPENSUBDIV_GPU=OFF`. From the Nuparu checkout:

```bat
set NUPARU_REPO=%CD%
set NUPARU_ARCH=full-windows
git submodule update --init --recursive
cmake -S . -B "build\%NUPARU_ARCH%" -G Ninja ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_INSTALL_PREFIX="%NUPARU_REPO%\packages\%NUPARU_ARCH%" ^
  -DNUPARU_BUILD_ALL=ON ^
  -DNUPARU_OPENSUBDIV_GPU=ON ^
  -DCUDAToolkit_ROOT="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.5" ^
  -DCMAKE_CUDA_COMPILER="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.5\bin\nvcc.exe" ^
  -DNUPARU_OPENSUBDIV_CUDA_ARCHITECTURES=75
cmake --build "build\%NUPARU_ARCH%" -j 4
ctest --test-dir "build\%NUPARU_ARCH%" --output-on-failure
```

Windows package libraries use the static MSVC runtime; TBB itself remains a DLL.

## Versions

Nuparu currently contains dependencies at the following versions:

|  |  |
|---|---:|
| [Blosc (c-blosc)](https://github.com/Blosc) | 1.21.6 |
| [Boost](https://github.com/boostorg/boost) | 1.92.0, selected subset |
| [cereal](https://uscilab.github.io/cereal/) | 1.3.2, with custom compatibility patch |
| [cppzmq](https://github.com/zeromq/cppzmq) | 4.10.0 |
| [cpuinfo](https://github.com/pytorch/cpuinfo) | Commit e4cadd02 |
| [Embree](https://embree.github.io/) | 4.4.0 |
| [GLEW](https://github.com/nigels-com/glew) | 2.2.0 |
| [GLFW](http://www.glfw.org/) | 3.4 |
| [GLM](http://glm.g-truc.net/) | 1.0.1 |
| [Dear ImGui](https://github.com/ocornut/imgui) | 1.92.0 docking |
| [imnodes](https://github.com/Nelarius/imnodes) | 0.5, with custom compatibility patch |
| [Imath](https://github.com/AcademySoftwareFoundation/Imath) | 3.1.12 |
| [jsoncpp](https://github.com/open-source-parsers/jsoncpp) | 1.9.6 |
| [libdeflate](https://github.com/ebiggers/libdeflate) | 1.18, embedded in OpenEXR |
| [NanoVDB](https://github.com/AcademySoftwareFoundation/openvdb) | From OpenVDB 12.0.1; NanoVDB’s own version is 32.7.0 |
| [oneTBB](https://github.com/uxlfoundation/onetbb) | 2022.1.0 |
| [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr) | 3.3.4 |
| [OpenSubdiv](https://github.com/pixaranimationstudios/opensubdiv) | 3.8.0 |
| [OpenVDB](https://github.com/AcademySoftwareFoundation/openvdb) | 12.0.1 |
| [Partio](https://github.com/wdas/partio) | 1.19.0 |
| [Ptex](https://github.com/wdas/ptex) | 2.4.3 |
| [RMSD](http://boscoh.com/code/) | 1.0 |
| [Sobol generator](http://gruenschloss.org/) | 1.0 |
| [stb](https://github.com/nothings/stb) | Commit 013ac3b: stb_image 2.30 and stb_image_write 1.16 |
| [tinyformat](https://github.com/c42f/tinyformat) | 2.3.0 |
| [zlib](https://www.zlib.net/) | 1.3.1 |
| [ZeroMQ (libzmq)](https://github.com/zeromq/libzmq) | 4.3.5 |
