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
foreach(dependency jsoncpp glfw)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/${dependency}/CMakeLists.txt")
        message(FATAL_ERROR "${dependency} submodule is missing. Run: git submodule update --init --recursive")
    endif()
endforeach()

set(NUPARU_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/stage")
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

add_custom_target(nuparu-package ALL
    COMMAND ${CMAKE_COMMAND}
        "-DNUPARU_STAGE_DIR=${NUPARU_STAGE_DIR}"
        "-DNUPARU_PACKAGE_DIR=${CMAKE_INSTALL_PREFIX}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/assemble-package.cmake"
    DEPENDS nuparu-jsoncpp nuparu-glfw
    VERBATIM
)

enable_testing()
add_test(NAME package-assembly
    COMMAND ${CMAKE_COMMAND}
        "-DASSEMBLER=${CMAKE_CURRENT_SOURCE_DIR}/cmake/assemble-package.cmake"
        "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/test-package-assembly"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/package-assembly.cmake")
