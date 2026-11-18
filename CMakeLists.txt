cmake_minimum_required(VERSION 3.20)
project(NuparuDependencies LANGUAGES C CXX)

include(ExternalProject)

if(NOT APPLE)
    message(FATAL_ERROR "The dependency pilot currently supports macOS only")
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
string(REPLACE ";" "|" NUPARU_OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}")

set(NUPARU_JSONCPP_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/deps/jsoncpp")
set(NUPARU_GLFW_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw")
foreach(dependency jsoncpp glfw zlib Imath openexr libdeflate)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/${dependency}/CMakeLists.txt")
        message(FATAL_ERROR "${dependency} submodule is missing. Run: git submodule update --init --recursive")
    endif()
endforeach()

set(NUPARU_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/stage")
# OpenEXR adjusts libdeflate's symbol visibility in its source tree. Use a
# build-local copy so configuring it cannot modify the pinned submodule.
foreach(dependency libdeflate zlib)
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
    -DCMAKE_OSX_ARCHITECTURES:STRING=${NUPARU_OSX_ARCHITECTURES}
    -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
    -DBUILD_SHARED_LIBS:BOOL=OFF)

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
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <BINARY_DIR>/libz.a ${NUPARU_STAGE_DIR}/zlib/lib/libz.a
    COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/LICENSE ${NUPARU_STAGE_DIR}/zlib/share/licenses/zlib/LICENSE
    LIST_SEPARATOR |
)

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

add_custom_target(nuparu-package ALL
    COMMAND ${CMAKE_COMMAND}
        "-DNUPARU_STAGE_DIR=${NUPARU_STAGE_DIR}"
        "-DNUPARU_PACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/assemble-package.cmake"
    DEPENDS nuparu-jsoncpp nuparu-glfw nuparu-zlib nuparu-Imath nuparu-openexr
    VERBATIM
)

enable_testing()
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
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/package-consumer.cmake")
