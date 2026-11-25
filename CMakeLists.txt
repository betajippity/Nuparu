cmake_minimum_required(VERSION 3.20)
project(NuparuDependencies LANGUAGES C CXX)

include(ExternalProject)
option(NUPARU_BUILD_EMBREE "Build the experimental native Embree package" OFF)
option(NUPARU_BUILD_TBB "Build and package shared oneTBB" OFF)
option(NUPARU_BUILD_BOOST "Build the Boost subset used by Takua and legacy OpenVDB" OFF)
option(NUPARU_BUILD_PTEX "Build the Ptex pilot" OFF)
option(NUPARU_BUILD_OPENSUBDIV "Build the OpenSubdiv pilot" OFF)
option(NUPARU_OPENSUBDIV_GPU "Enable the selected OpenSubdiv GPU backends" ON)
set(_nuparu_osd_cuda_default OFF)
if((WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux") AND CMAKE_SIZEOF_VOID_P EQUAL 8 AND
   CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64)$")
    set(_nuparu_osd_cuda_default ON)
endif()
option(NUPARU_OPENSUBDIV_CUDA "Build the OpenSubdiv CUDA backend" ${_nuparu_osd_cuda_default})
option(NUPARU_OPENSUBDIV_OPENGL "Build the OpenSubdiv OpenGL backend" ON)
option(NUPARU_OPENSUBDIV_METAL "Build the OpenSubdiv Metal backend on macOS" ${APPLE})
set(NUPARU_OPENSUBDIV_CUDA_ARCHITECTURES "52" CACHE STRING "CUDA architectures for OpenSubdiv")
set(NUPARU_BOOST_LEGACY_OPENVDB_ROOT "" CACHE PATH "Optional legacy Nuparu prefix for testing OpenVDB with packaged Boost")
set(NUPARU_TBB_ROOT "" CACHE PATH "Legacy Nuparu platform prefix supplying TBB")
set(NUPARU_ISPC_EXECUTABLE "" CACHE FILEPATH "Optional override for the ISPC compiler")
set(NUPARU_ISPC_ARCHIVE "" CACHE FILEPATH "Optional local ISPC 1.32.0 release archive")
set(NUPARU_ISPC_CACHE_DIR "${CMAKE_BINARY_DIR}/ispc-cache" CACHE PATH "Cache for downloaded ISPC tools")
option(NUPARU_ISPC_AUTO_DOWNLOAD "Download the pinned ISPC tool when Embree is enabled" ON)
set(NUPARU_EMBREE_CLANG_CL "" CACHE FILEPATH "Windows ARM64 clang-cl executable for Embree")

if(NOT APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT WIN32)
    message(FATAL_ERROR "The dependency pilot supports macOS, Linux and Windows")
endif()
if(CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "The jsoncpp pilot requires a single-configuration generator such as Ninja")
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(FATAL_ERROR "Set CMAKE_INSTALL_PREFIX to a dedicated Nuparu package directory")
endif()
if(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "Set CMAKE_BUILD_TYPE explicitly for the Nuparu pilot")
endif()
if(WIN32)
    if(NOT MSVC OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(FATAL_ERROR "The Windows Nuparu package requires the MSVC compiler; clang-cl is used separately for ARM64 Embree")
    endif()
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
        message(FATAL_ERROR "The Windows Nuparu pilot currently supports Release builds only")
    endif()
    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "The Windows Nuparu pilot supports x86-64 and ARM64 only")
    endif()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ARM64|arm64|aarch64)$" AND
       CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "^(x64|X64|AMD64|x86_64)$")
        message(FATAL_ERROR "Use an ARM64 native tools prompt for a Windows ARM64 package")
    endif()
endif()
if(APPLE)
    string(REPLACE ";" "|" NUPARU_OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}")
endif()

set(NUPARU_JSONCPP_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/deps/jsoncpp")
set(NUPARU_GLFW_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw")
foreach(dependency jsoncpp glfw zlib Imath openexr libdeflate)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/${dependency}/CMakeLists.txt")
        message(FATAL_ERROR "${dependency} submodule is missing. Run: git submodule update --init --recursive")
    endif()
endforeach()
if(NUPARU_BUILD_EMBREE AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/embree/CMakeLists.txt")
    message(FATAL_ERROR "Embree submodule is missing. Run: git submodule update --init --recursive")
endif()
if(NUPARU_BUILD_TBB AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/onetbb/CMakeLists.txt")
    message(FATAL_ERROR "oneTBB submodule is missing. Run: git submodule update --init --recursive")
endif()
if(NUPARU_BUILD_PTEX AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/ptex/CMakeLists.txt")
    message(FATAL_ERROR "Ptex submodule is missing. Run: git submodule update --init --recursive")
endif()
if(NUPARU_BUILD_OPENSUBDIV)
    if(NOT NUPARU_BUILD_PTEX OR NOT NUPARU_BUILD_TBB)
        message(FATAL_ERROR "OpenSubdiv requires NUPARU_BUILD_PTEX=ON and NUPARU_BUILD_TBB=ON")
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/opensubdiv/CMakeLists.txt")
        message(FATAL_ERROR "OpenSubdiv submodule is missing. Run: git submodule update --init --recursive")
    endif()
endif()
if(NUPARU_BUILD_BOOST)
    foreach(boost_file CMakeLists.txt tools/cmake/include/BoostRoot.cmake libs/headers/CMakeLists.txt libs/filesystem/CMakeLists.txt libs/random/CMakeLists.txt libs/iostreams/CMakeLists.txt)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/boost/${boost_file}")
            message(FATAL_ERROR "Boost sources are incomplete (${boost_file}). Run: git submodule update --init --recursive")
        endif()
    endforeach()
endif()

set(NUPARU_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/stage")
# OpenEXR adjusts libdeflate's symbol visibility in its source tree. Use a
# build-local copy so configuring it cannot modify the pinned submodule.
set(nuparu_copied_sources libdeflate zlib)
if(NUPARU_BUILD_PTEX)
    list(APPEND nuparu_copied_sources ptex)
endif()
if(WIN32 AND NUPARU_BUILD_OPENSUBDIV)
    list(APPEND nuparu_copied_sources opensubdiv)
endif()
foreach(dependency IN LISTS nuparu_copied_sources)
    # A fresh build directory is required when changing either source revision.
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/sources/${dependency}/CMakeLists.txt")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deps/${dependency}/"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/sources/${dependency}"
            PATTERN ".git" EXCLUDE PATTERN "zconf.h.included" EXCLUDE)
    endif()
endforeach()
set(NUPARU_COMMON_ARGS
    -DCMAKE_INSTALL_LIBDIR:STRING=lib
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS:BOOL=OFF)
if(WIN32)
    # Several upstream projects set their minimum CMake version below 3.15.
    # Make the runtime-library setting effective before their project() calls.
    list(APPEND NUPARU_COMMON_ARGS -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
        -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=MultiThreaded)
    set(nuparu_jsoncpp_windows_args -DJSONCPP_STATIC_WINDOWS_RUNTIME:BOOL=ON)
    set(nuparu_glfw_windows_args -DUSE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF)
    set(nuparu_embree_windows_args -DEMBREE_STATIC_RUNTIME:BOOL=ON)
    set(nuparu_zlib_build_name zlibstatic.lib)
    set(nuparu_zlib_package_name z.lib)
else()
    set(nuparu_zlib_build_name libz.a)
    set(nuparu_zlib_package_name libz.a)
endif()
if(APPLE)
    list(APPEND NUPARU_COMMON_ARGS
        -DCMAKE_OSX_ARCHITECTURES:STRING=${NUPARU_OSX_ARCHITECTURES}
        -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT})
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Takua's current Linux viewer and link settings use X11.
    set(NUPARU_GLFW_PLATFORM_ARGS -DGLFW_BUILD_X11:BOOL=ON -DGLFW_BUILD_WAYLAND:BOOL=OFF)
endif()

if(NUPARU_BUILD_TBB)
    ExternalProject_Add(nuparu-tbb
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/onetbb"
        DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/tbb
            -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
            ${NUPARU_COMMON_ARGS} -DBUILD_SHARED_LIBS:BOOL=ON
            -DTBB_TEST:BOOL=OFF -DTBB_EXAMPLES:BOOL=OFF -DTBB_STRICT:BOOL=OFF
        LIST_SEPARATOR |
    )
    ExternalProject_Add_Step(nuparu-tbb install-license
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/tbb/share/licenses/onetbb
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE.txt ${NUPARU_STAGE_DIR}/tbb/share/licenses/onetbb/LICENSE.txt
        DEPENDEES install
    )
    set(nuparu_tbb_dependency nuparu-tbb)
endif()

if(NUPARU_BUILD_BOOST)
    ExternalProject_Add(nuparu-boost
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/boost"
        DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/boost
            -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
            ${NUPARU_COMMON_ARGS}
            -DBOOST_INCLUDE_LIBRARIES:STRING=filesystem|random|iostreams
            -DBOOST_ENABLE_PYTHON:BOOL=OFF -DBOOST_ENABLE_MPI:BOOL=OFF
            -DBOOST_RANDOM_ENABLE_RANDOM_DEVICE:BOOL=OFF
            -DBOOST_IOSTREAMS_ENABLE_ZLIB:BOOL=OFF -DBOOST_IOSTREAMS_ENABLE_BZIP2:BOOL=OFF
            -DBOOST_IOSTREAMS_ENABLE_LZMA:BOOL=OFF -DBOOST_IOSTREAMS_ENABLE_ZSTD:BOOL=OFF
            -DCMAKE_DISABLE_FIND_PACKAGE_ICU:BOOL=ON
            -DBOOST_INSTALL_LAYOUT:STRING=system -DBUILD_TESTING:BOOL=OFF
        LIST_SEPARATOR |
    )
    ExternalProject_Add_Step(nuparu-boost install-license
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/boost/share/licenses/boost
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE_1_0.txt ${NUPARU_STAGE_DIR}/boost/share/licenses/boost/LICENSE_1_0.txt
        DEPENDEES install
    )
    set(nuparu_boost_dependency nuparu-boost)
endif()

ExternalProject_Add(nuparu-jsoncpp
    SOURCE_DIR "${NUPARU_JSONCPP_SOURCE}"
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    # Let the nested build detect edits or a changed submodule checkout.
    BUILD_ALWAYS TRUE
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/jsoncpp
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        ${NUPARU_COMMON_ARGS}
        -DJSONCPP_WITH_TESTS:BOOL=OFF
        -DJSONCPP_WITH_PKGCONFIG_SUPPORT:BOOL=OFF
        -DJSONCPP_WITH_CMAKE_PACKAGE:BOOL=ON
        ${nuparu_jsoncpp_windows_args}
        -DBUILD_STATIC_LIBS:BOOL=ON
        -DBUILD_OBJECT_LIBS:BOOL=OFF
    LIST_SEPARATOR |
)
ExternalProject_Add_Step(nuparu-jsoncpp install-license
    COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/jsoncpp/share/licenses/jsoncpp
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE ${NUPARU_STAGE_DIR}/jsoncpp/share/licenses/jsoncpp/LICENSE
    DEPENDEES install
)

ExternalProject_Add(nuparu-glfw
    SOURCE_DIR "${NUPARU_GLFW_SOURCE}"
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    BUILD_ALWAYS TRUE
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/glfw
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        ${NUPARU_COMMON_ARGS}
        -DGLFW_BUILD_EXAMPLES:BOOL=OFF
        -DGLFW_BUILD_TESTS:BOOL=OFF
        -DGLFW_BUILD_DOCS:BOOL=OFF
        -DGLFW_INSTALL:BOOL=ON
        ${nuparu_glfw_windows_args}
        ${NUPARU_GLFW_PLATFORM_ARGS}
    LIST_SEPARATOR |
)
ExternalProject_Add_Step(nuparu-glfw install-license
    COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/glfw/share/licenses/glfw
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE.md ${NUPARU_STAGE_DIR}/glfw/share/licenses/glfw/LICENSE.md
    DEPENDEES install
)

ExternalProject_Add(nuparu-zlib
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/sources/zlib"
    DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/zlib
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        ${NUPARU_COMMON_ARGS}
        -DZLIB_BUILD_EXAMPLES:BOOL=OFF
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target zlibstatic
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/zlib/include ${NUPARU_STAGE_DIR}/zlib/lib ${NUPARU_STAGE_DIR}/zlib/share/licenses/zlib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/zlib.h <BINARY_DIR>/zconf.h ${NUPARU_STAGE_DIR}/zlib/include
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <BINARY_DIR>/${nuparu_zlib_build_name} ${NUPARU_STAGE_DIR}/zlib/lib/${nuparu_zlib_package_name}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE ${NUPARU_STAGE_DIR}/zlib/share/licenses/zlib/LICENSE
    LIST_SEPARATOR |
)

if(NUPARU_BUILD_PTEX)
    ExternalProject_Add(nuparu-ptex
        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/sources/ptex"
        DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
        DEPENDS nuparu-zlib
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/ptex
            -DCMAKE_INSTALL_INCLUDEDIR:STRING=include/ptex
            -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
            ${NUPARU_COMMON_ARGS}
            -DCMAKE_CXX_STANDARD:STRING=17 -DPTEX_VER:STRING=v2.4.3
            -DPTEX_SHA:STRING=4aa2c35 -DPTEX_BUILD_STATIC_LIBS:BOOL=ON
            -DPTEX_BUILD_SHARED_LIBS:BOOL=OFF -DPTEX_BUILD_DOCS:BOOL=OFF
            -DZLIB_INCLUDE_DIR:PATH=${NUPARU_STAGE_DIR}/zlib/include
            -DZLIB_LIBRARY:FILEPATH=${NUPARU_STAGE_DIR}/zlib/lib/${nuparu_zlib_package_name}
        LIST_SEPARATOR |
    )
    ExternalProject_Add_Step(nuparu-ptex install-license
        COMMAND ${CMAKE_COMMAND} "-DPTEX_ROOT=${NUPARU_STAGE_DIR}/ptex" "-DPTEX_CACHE=<BINARY_DIR>/CMakeCache.txt"
            "-DEXPECTED_ZLIB_INCLUDE=${NUPARU_STAGE_DIR}/zlib/include"
            "-DEXPECTED_ZLIB_LIBRARY=${NUPARU_STAGE_DIR}/zlib/lib/${nuparu_zlib_package_name}"
            "-DPTEX_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/prepare-ptex.cmake"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/ptex/share/licenses/ptex
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE ${NUPARU_STAGE_DIR}/ptex/share/licenses/ptex/LICENSE
        DEPENDEES install
    )
    set(nuparu_ptex_dependency nuparu-ptex)
endif()

if(NUPARU_BUILD_OPENSUBDIV)
    if(WIN32)
        set(nuparu_ptex_library "${NUPARU_STAGE_DIR}/ptex/lib/Ptex.lib")
    else()
        set(nuparu_ptex_library "${NUPARU_STAGE_DIR}/ptex/lib/libPtex.a")
    endif()
    foreach(backend CUDA OPENGL METAL)
        if(NUPARU_OPENSUBDIV_GPU AND NUPARU_OPENSUBDIV_${backend})
            set(nuparu_osd_${backend} ON)
            set(nuparu_osd_no_${backend} OFF)
        else()
            set(nuparu_osd_${backend} OFF)
            set(nuparu_osd_no_${backend} ON)
        endif()
    endforeach()
    if(nuparu_osd_METAL AND NOT APPLE)
        message(FATAL_ERROR "The OpenSubdiv Metal backend requires macOS")
    endif()
    if(nuparu_osd_CUDA AND NOT NUPARU_OPENSUBDIV_CUDA_ARCHITECTURES)
        message(FATAL_ERROR "Set NUPARU_OPENSUBDIV_CUDA_ARCHITECTURES when enabling CUDA")
    endif()
    string(REPLACE ";" "|" nuparu_cuda_architectures "${NUPARU_OPENSUBDIV_CUDA_ARCHITECTURES}")
    set(nuparu_osd_cuda_args)
    if(nuparu_osd_CUDA)
        list(APPEND nuparu_osd_cuda_args -DCMAKE_CUDA_ARCHITECTURES:STRING=${nuparu_cuda_architectures})
        foreach(variable CMAKE_CUDA_COMPILER CMAKE_CUDA_HOST_COMPILER CUDAToolkit_ROOT)
            if(DEFINED ${variable} AND NOT "${${variable}}" STREQUAL "")
                if(variable STREQUAL "CUDAToolkit_ROOT")
                    set(nuparu_cuda_variable_type PATH)
                else()
                    set(nuparu_cuda_variable_type FILEPATH)
                endif()
                list(APPEND nuparu_osd_cuda_args "-D${variable}:${nuparu_cuda_variable_type}=${${variable}}")
            endif()
        endforeach()
        if(DEFINED CMAKE_CUDA_FLAGS AND NOT CMAKE_CUDA_FLAGS STREQUAL "")
            list(APPEND nuparu_osd_cuda_args "-DCMAKE_CUDA_FLAGS:STRING=${CMAKE_CUDA_FLAGS}")
        endif()
    endif()
    if(WIN32)
        # Keep OpenSubdiv's static CRT branch, but remove its unqualified /MT
        # options from a build-local source copy. CMake selects /MT itself and
        # forwards it correctly through nvcc; the OFF branch suppresses libcmt.
        include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/PatchOpenSubdivWindows.cmake")
        set(nuparu_osd_source "${CMAKE_CURRENT_BINARY_DIR}/sources/opensubdiv")
        nuparu_patch_opensubdiv_windows("${nuparu_osd_source}/CMakeLists.txt")
        set(nuparu_osd_windows_args -DMSVC_STATIC_CRT:BOOL=ON)
    else()
        set(nuparu_osd_source "${CMAKE_CURRENT_SOURCE_DIR}/deps/opensubdiv")
    endif()
    ExternalProject_Add(nuparu-opensubdiv
        SOURCE_DIR "${nuparu_osd_source}"
        DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
        DEPENDS nuparu-ptex nuparu-tbb
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/opensubdiv
            -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
            ${NUPARU_COMMON_ARGS} -DCMAKE_CXX_STANDARD:STRING=17
            -DTBB_DIR:PATH=${NUPARU_STAGE_DIR}/tbb/lib/cmake/TBB
            -DPTEX_INCLUDE_DIR:PATH=${NUPARU_STAGE_DIR}/ptex/include/ptex
            -DPTEX_LIBRARY:FILEPATH=${nuparu_ptex_library}
            -DZLIB_INCLUDE_DIR:PATH=${NUPARU_STAGE_DIR}/zlib/include
            -DZLIB_LIBRARY:FILEPATH=${NUPARU_STAGE_DIR}/zlib/lib/${nuparu_zlib_package_name}
            -DNO_PTEX:BOOL=OFF -DNO_TBB:BOOL=OFF
            -DNO_CUDA:BOOL=${nuparu_osd_no_CUDA} ${nuparu_osd_cuda_args}
            -DNO_OPENGL:BOOL=${nuparu_osd_no_OPENGL} -DNO_METAL:BOOL=${nuparu_osd_no_METAL}
            -DNO_EXAMPLES:BOOL=ON -DNO_TUTORIALS:BOOL=ON -DNO_REGRESSION:BOOL=ON
            -DNO_TESTS:BOOL=ON -DNO_DOC:BOOL=ON -DNO_OMP:BOOL=ON
            -DNO_OPENCL:BOOL=ON -DNO_DX:BOOL=ON -DNO_GLFW:BOOL=ON
            -DNO_MACOS_FRAMEWORK:BOOL=ON ${nuparu_osd_windows_args}
        LIST_SEPARATOR |
    )
    ExternalProject_Add_Step(nuparu-opensubdiv verify-and-license
        COMMAND ${CMAKE_COMMAND}
            "-DOSD_ROOT=${NUPARU_STAGE_DIR}/opensubdiv" "-DOSD_CACHE=<BINARY_DIR>/CMakeCache.txt"
            "-DEXPECTED_TBB_DIR=${NUPARU_STAGE_DIR}/tbb/lib/cmake/TBB"
            "-DEXPECTED_PTEX_DIR=${NUPARU_STAGE_DIR}/ptex/include/ptex"
            "-DEXPECTED_PTEX_LIBRARY=${nuparu_ptex_library}"
            "-DEXPECT_OPENGL=${nuparu_osd_OPENGL}"
            "-DEXPECT_CUDA=${nuparu_osd_CUDA}" "-DEXPECT_METAL=${nuparu_osd_METAL}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/verify-opensubdiv.cmake"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/opensubdiv/share/licenses/opensubdiv
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE.txt <SOURCE_DIR>/NOTICE.txt ${NUPARU_STAGE_DIR}/opensubdiv/share/licenses/opensubdiv
        DEPENDEES install
    )
    set(nuparu_osd_dependency nuparu-opensubdiv)
endif()

ExternalProject_Add(nuparu-Imath
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/Imath"
    DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/Imath
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        ${NUPARU_COMMON_ARGS}
        -DBUILD_TESTING:BOOL=OFF -DPYTHON:BOOL=OFF
        -DIMATH_LIB_SUFFIX:STRING= -DIMATH_INSTALL_PKG_CONFIG:BOOL=OFF
    LIST_SEPARATOR |
)
ExternalProject_Add_Step(nuparu-Imath install-license
    COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/Imath/share/licenses/Imath
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE.md ${NUPARU_STAGE_DIR}/Imath/share/licenses/Imath/LICENSE.md
    DEPENDEES install
)

ExternalProject_Add(nuparu-openexr
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/openexr"
    DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
    DEPENDS nuparu-Imath nuparu-zlib
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/openexr
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        ${NUPARU_COMMON_ARGS}
        -DImath_DIR:PATH=${NUPARU_STAGE_DIR}/Imath/lib/cmake/Imath
        -DCMAKE_DISABLE_FIND_PACKAGE_Imath:BOOL=OFF
        -DOPENEXR_FORCE_INTERNAL_DEFLATE:BOOL=ON
        -DFETCHCONTENT_SOURCE_DIR_DEFLATE:PATH=${CMAKE_CURRENT_BINARY_DIR}/sources/libdeflate
        -DFETCHCONTENT_FULLY_DISCONNECTED:BOOL=ON
        -DBUILD_TESTING:BOOL=OFF -DOPENEXR_BUILD_TOOLS:BOOL=OFF
        -DOPENEXR_BUILD_EXAMPLES:BOOL=OFF -DOPENEXR_INSTALL_PKG_CONFIG:BOOL=OFF
        -DOPENEXR_LIB_SUFFIX:STRING=
    LIST_SEPARATOR |
)
ExternalProject_Add_Step(nuparu-openexr verify-imath
    COMMAND ${CMAKE_COMMAND}
        "-DOPENEXR_CACHE=<BINARY_DIR>/CMakeCache.txt"
        "-DEXPECTED_IMATH_DIR=${NUPARU_STAGE_DIR}/Imath/lib/cmake/Imath"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/verify-imath.cmake"
    DEPENDEES configure
    DEPENDERS build
)
ExternalProject_Add_Step(nuparu-openexr install-license
    COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/openexr/share/licenses/openexr ${NUPARU_STAGE_DIR}/openexr/share/licenses/libdeflate
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE.md ${NUPARU_STAGE_DIR}/openexr/share/licenses/openexr/LICENSE.md
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdeflate/COPYING ${NUPARU_STAGE_DIR}/openexr/share/licenses/libdeflate/COPYING
    DEPENDEES install
)

if(NUPARU_BUILD_EMBREE)
    if(CMAKE_CROSSCOMPILING)
        message(FATAL_ERROR "The Embree pilot currently requires a native build")
    endif()
    if(APPLE AND CMAKE_OSX_ARCHITECTURES)
        list(LENGTH CMAKE_OSX_ARCHITECTURES architecture_count)
        if(NOT architecture_count EQUAL 1)
            message(FATAL_ERROR "The Embree pilot requires one macOS architecture per build; universal assembly is not implemented")
        endif()
        set(embree_arch "${CMAKE_OSX_ARCHITECTURES}")
    elseif(WIN32 AND CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "^(ARM64|arm64)$")
        set(embree_arch ARM64)
        if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
            message(FATAL_ERROR "Windows ARM64 Embree requires an ARM64 native developer environment (CMAKE_SYSTEM_PROCESSOR must be ARM64)")
        endif()
    else()
        set(embree_arch "${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/EmbreeISA.cmake")
    nuparu_embree_isa("${embree_arch}" "${APPLE}" "${WIN32}" embree_isa_args)
    if(NUPARU_BUILD_TBB)
        set(embree_tbb_version_header "${CMAKE_CURRENT_SOURCE_DIR}/deps/onetbb/include/oneapi/tbb/version.h")
        set(embree_tbb_dir "${NUPARU_STAGE_DIR}/tbb/lib/cmake/TBB")
        set(embree_tbb_kind package)
    elseif(NOT NUPARU_TBB_ROOT OR NOT EXISTS "${NUPARU_TBB_ROOT}/include/oneapi/tbb/version.h")
        message(FATAL_ERROR "Set NUPARU_TBB_ROOT to the legacy Nuparu platform prefix containing TBB")
    else()
        set(embree_tbb_version_header "${NUPARU_TBB_ROOT}/include/oneapi/tbb/version.h")
        set(embree_tbb_dir "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NuparuTBB")
        set(embree_tbb_kind legacy)
    endif()
    if(NUPARU_ISPC_EXECUTABLE)
        set(nuparu_ispc_executable "${NUPARU_ISPC_EXECUTABLE}")
    else()
        include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ISPC.cmake")
        nuparu_resolve_ispc(nuparu_ispc_executable)
    endif()
    if(NOT EXISTS "${nuparu_ispc_executable}")
        message(FATAL_ERROR "ISPC executable does not exist: ${nuparu_ispc_executable}")
    endif()
    execute_process(COMMAND "${nuparu_ispc_executable}" --version
        RESULT_VARIABLE ispc_result OUTPUT_VARIABLE ispc_version_output ERROR_VARIABLE ispc_error)
    if(NOT ispc_result EQUAL 0)
        message(FATAL_ERROR "Cannot run ISPC ${nuparu_ispc_executable}: ${ispc_error}")
    endif()
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" ispc_version "${ispc_version_output}")
    if(NOT ispc_version)
        message(FATAL_ERROR "Could not determine ISPC version from: ${ispc_version_output}")
    endif()
    if(NOT NUPARU_ISPC_EXECUTABLE AND NOT ispc_version VERSION_EQUAL "1.32.0")
        message(FATAL_ERROR "Expected downloaded ISPC 1.32.0, found ${ispc_version}")
    endif()
    foreach(part MAJOR MINOR PATCH)
        file(STRINGS "${embree_tbb_version_header}" tbb_version_line
            REGEX "^#define TBB_VERSION_${part} +[0-9]+")
        string(REGEX REPLACE ".* +([0-9]+).*" "\\1" tbb_version_${part} "${tbb_version_line}")
    endforeach()
    set(embree_tbb_version "${tbb_version_MAJOR}.${tbb_version_MINOR}.${tbb_version_PATCH}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/NuparuTBBExpected.cmake"
        "set(NUPARU_EMBREE_TBB_VERSION \"${embree_tbb_version}\")\nset(NUPARU_EMBREE_ISPC_VERSION \"${ispc_version}\")\nset(NUPARU_EMBREE_TBB_KIND \"${embree_tbb_kind}\")\n")
    message(STATUS "Embree pilot: ${embree_arch}, ISPC ${ispc_version}, ${embree_tbb_kind} TBB ${embree_tbb_version}")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/sources/embree/CMakeLists.txt")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deps/embree/"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/sources/embree" PATTERN ".git" EXCLUDE)
    endif()
    if(NUPARU_BUILD_TBB)
        # Upstream's dependency lookup otherwise requires tbbmalloc_proxy,
        # which oneTBB does not build on Windows ARM64.
        set(embree_config_template "${CMAKE_CURRENT_BINARY_DIR}/sources/embree/common/cmake/embree-config.cmake")
        file(READ "${embree_config_template}" embree_config_contents)
        string(REPLACE "FIND_DEPENDENCY(TBB)" "FIND_DEPENDENCY(TBB COMPONENTS tbb)" patched_embree_config "${embree_config_contents}")
        if(NOT patched_embree_config MATCHES "FIND_DEPENDENCY\\(TBB COMPONENTS tbb\\)")
            message(FATAL_ERROR "Cannot patch Embree's TBB dependency; check the Embree revision")
        endif()
        if(NOT patched_embree_config STREQUAL embree_config_contents)
            file(WRITE "${embree_config_template}" "${patched_embree_config}")
        endif()
    endif()
    set(embree_compiler_args
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER})
    if(WIN32 AND embree_arch STREQUAL "ARM64")
        if(NUPARU_EMBREE_CLANG_CL)
            set(embree_clang_cl "${NUPARU_EMBREE_CLANG_CL}")
        else()
            find_program(embree_clang_cl NAMES clang-cl REQUIRED)
        endif()
        set(embree_compiler_args
            -DCMAKE_C_COMPILER:FILEPATH=${embree_clang_cl}
            -DCMAKE_CXX_COMPILER:FILEPATH=${embree_clang_cl}
            -DCMAKE_C_FLAGS_INIT:STRING=--target=arm64-pc-windows-msvc
            -DCMAKE_CXX_FLAGS_INIT:STRING=--target=arm64-pc-windows-msvc)
        # Embree detects ARM64 but its ISPC helper only matches lowercase names.
        # Patch the build-local source copy, leaving the submodule untouched.
        set(ispc_cmake "${CMAKE_CURRENT_BINARY_DIR}/sources/embree/common/cmake/ispc.cmake")
        file(READ "${ispc_cmake}" ispc_cmake_contents)
        string(REPLACE "MATCHES \"arm64|aarch64\"" "MATCHES \"arm64|aarch64|ARM64\"" patched_ispc_cmake "${ispc_cmake_contents}")
        if(NOT patched_ispc_cmake MATCHES "arm64\\|aarch64\\|ARM64")
            message(FATAL_ERROR "Cannot patch Embree's ISPC architecture detection; check the Embree revision")
        endif()
        if(NOT patched_ispc_cmake STREQUAL ispc_cmake_contents)
            file(WRITE "${ispc_cmake}" "${patched_ispc_cmake}")
        endif()
        include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/PatchEmbreeWinArm64.cmake")
        nuparu_patch_embree_win_arm64("${CMAKE_CURRENT_BINARY_DIR}/sources/embree/common/simd/arm/sse2neon.h")
    endif()
    ExternalProject_Add(nuparu-embree
        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/sources/embree"
        DOWNLOAD_COMMAND "" UPDATE_COMMAND "" BUILD_ALWAYS TRUE
        DEPENDS ${nuparu_tbb_dependency}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${NUPARU_STAGE_DIR}/embree
            ${embree_compiler_args}
            ${NUPARU_COMMON_ARGS}
            -DEMBREE_STATIC_LIB:BOOL=ON -DEMBREE_TUTORIALS:BOOL=OFF ${nuparu_embree_windows_args}
            -DEMBREE_SYCL_SUPPORT:BOOL=OFF -DEMBREE_ISPC_SUPPORT:BOOL=ON
            -DEMBREE_ISPC_EXECUTABLE:FILEPATH=${nuparu_ispc_executable}
            -DEMBREE_MAX_INSTANCE_LEVEL_COUNT:STRING=16 -DEMBREE_MAX_ISA:STRING=NONE
            ${embree_isa_args}
            -DEMBREE_TASKING_SYSTEM:STRING=TBB -DEMBREE_INSTALL_DEPENDENCIES:BOOL=OFF
            -DNUPARU_TBB_ROOT:PATH=${NUPARU_TBB_ROOT}
            -DTBB_DIR:PATH=${embree_tbb_dir}
        LIST_SEPARATOR |
    )
    ExternalProject_Add_Step(nuparu-embree install-adapter
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NUPARU_STAGE_DIR}/embree/lib/cmake/NuparuTBB ${NUPARU_STAGE_DIR}/embree/share/licenses/embree
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NuparuTBB/TBBConfig.cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NuparuTBB/TBBConfigVersion.cmake ${NUPARU_STAGE_DIR}/embree/lib/cmake/NuparuTBB
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/NuparuTBBExpected.cmake ${NUPARU_STAGE_DIR}/embree/lib/cmake/NuparuTBB/NuparuTBBExpected.cmake
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE.txt ${NUPARU_STAGE_DIR}/embree/share/licenses/embree/LICENSE.txt
        DEPENDEES install
    )
    set(nuparu_embree_dependency nuparu-embree)
endif()

add_custom_target(nuparu-package ALL
    COMMAND ${CMAKE_COMMAND}
        "-DNUPARU_STAGE_DIR=${NUPARU_STAGE_DIR}"
        "-DNUPARU_PACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
        "-DNUPARU_INCLUDE_EMBREE=${NUPARU_BUILD_EMBREE}"
        "-DNUPARU_INCLUDE_TBB=${NUPARU_BUILD_TBB}"
        "-DNUPARU_INCLUDE_BOOST=${NUPARU_BUILD_BOOST}"
        "-DNUPARU_INCLUDE_PTEX=${NUPARU_BUILD_PTEX}"
        "-DNUPARU_INCLUDE_OPENSUBDIV=${NUPARU_BUILD_OPENSUBDIV}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/assemble-package.cmake"
    DEPENDS nuparu-jsoncpp nuparu-glfw nuparu-zlib nuparu-Imath nuparu-openexr ${nuparu_tbb_dependency} ${nuparu_embree_dependency} ${nuparu_boost_dependency} ${nuparu_ptex_dependency} ${nuparu_osd_dependency}
    VERBATIM
)

enable_testing()
foreach(dependency ptex opensubdiv)
    string(TOUPPER "${dependency}" dependency_upper)
    if(NUPARU_BUILD_${dependency_upper})
        add_test(NAME ${dependency}-consumer
            COMMAND ${CMAKE_COMMAND}
                "-DPACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
                "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-${dependency}-consumer"
                "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/${dependency}-consumer"
                "-DTEST_GENERATOR=${CMAKE_GENERATOR}" "-DTEST_NAME=${dependency}"
                "-DZLIB_NAME=${nuparu_zlib_package_name}"
                "-DEXPECT_CUDA=${nuparu_osd_CUDA}"
                "-DTEST_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/graphics-dependency-consumer.cmake")
    endif()
endforeach()
if(NUPARU_BUILD_BOOST)
    add_test(NAME boost-consumer
        COMMAND ${CMAKE_COMMAND}
            "-DPACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-boost-consumer"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/boost-consumer"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/boost-consumer.cmake")
    if(NUPARU_BOOST_LEGACY_OPENVDB_ROOT)
        if(NOT NUPARU_BUILD_TBB)
            message(FATAL_ERROR "Legacy OpenVDB / Boost test requires NUPARU_BUILD_TBB=ON")
        endif()
        add_test(NAME legacy-openvdb-boost
            COMMAND ${CMAKE_COMMAND}
                "-DPACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
                "-DLEGACY_ROOT=${NUPARU_BOOST_LEGACY_OPENVDB_ROOT}"
                "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-legacy-openvdb-boost"
                "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/legacy-openvdb-boost"
                "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
                "-DTEST_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
                "-DTEST_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}"
                "-DTEST_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/legacy-openvdb-boost.cmake")
    endif()
endif()
if(NUPARU_BUILD_TBB)
    add_test(NAME tbb-consumer
        COMMAND ${CMAKE_COMMAND}
            "-DPACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-tbb-consumer"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/tbb-consumer"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/tbb-consumer.cmake")
endif()
add_test(NAME ispc-release-selection
    COMMAND ${CMAKE_COMMAND} "-DISPC_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/cmake/ISPC.cmake"
        "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-ispc"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/ispc.cmake")
add_test(NAME embree-isa-selection
    COMMAND ${CMAKE_COMMAND} "-DISA_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/cmake/EmbreeISA.cmake"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/embree-isa.cmake")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/embree/common/simd/arm/sse2neon.h")
    add_test(NAME embree-win-arm64-patch
        COMMAND ${CMAKE_COMMAND}
            "-DPATCH_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/cmake/PatchEmbreeWinArm64.cmake"
            "-DEMBREE_HEADER=${CMAKE_CURRENT_SOURCE_DIR}/deps/embree/common/simd/arm/sse2neon.h"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-embree-win-arm64-patch"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DTEST_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/embree-win-arm64-patch.cmake")
endif()
add_test(NAME tbb-windows-adapter
    COMMAND ${CMAKE_COMMAND}
        "-DTBB_CONFIG=${CMAKE_CURRENT_SOURCE_DIR}/cmake/NuparuTBB/TBBConfig.cmake"
        "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-tbb-windows"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/tbb-windows.cmake")
add_test(NAME package-assembly
    COMMAND ${CMAKE_COMMAND}
        "-DASSEMBLER=${CMAKE_CURRENT_SOURCE_DIR}/cmake/assemble-package.cmake"
        "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-package-assembly"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/package-assembly.cmake")
add_test(NAME package-consumer
    COMMAND ${CMAKE_COMMAND}
        "-DPACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
        "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-package-consumer"
        "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/package-consumer"
        "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
        "-DTEST_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/package-consumer.cmake")
if(NUPARU_BUILD_EMBREE)
    add_test(NAME embree-consumer
        COMMAND ${CMAKE_COMMAND}
            "-DPACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-embree-consumer"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/embree-consumer"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
            "-DNUPARU_TBB_ROOT=${NUPARU_TBB_ROOT}"
            "-DNUPARU_BUILD_TBB=${NUPARU_BUILD_TBB}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/embree-consumer.cmake")
endif()
