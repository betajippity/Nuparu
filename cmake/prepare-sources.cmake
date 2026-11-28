cmake_minimum_required(VERSION 3.20)
# Recreate only this build's generated source stage, leaving the repository and
# the pinned submodules untouched.
file(REMOVE_RECURSE "${SOURCE_STAGE}")
file(MAKE_DIRECTORY "${SOURCE_STAGE}/src/imgui" "${SOURCE_STAGE}/src/imnodes"
    "${SOURCE_STAGE}/src/rmsd" "${SOURCE_STAGE}/src/sobol"
    "${SOURCE_STAGE}/include" "${SOURCE_STAGE}/lib/cmake/NuparuSources"
    "${SOURCE_STAGE}/share/licenses/imgui" "${SOURCE_STAGE}/share/licenses/imnodes")
file(GLOB imgui_core "${SOURCE_ROOT}/deps/imgui/*.cpp" "${SOURCE_ROOT}/deps/imgui/*.h")
file(COPY ${imgui_core} DESTINATION "${SOURCE_STAGE}/src/imgui")
foreach(backend glfw opengl3 metal osx vulkan)
    file(GLOB backend_files "${SOURCE_ROOT}/deps/imgui/backends/imgui_impl_${backend}.*")
    file(COPY ${backend_files} DESTINATION "${SOURCE_STAGE}/src/imgui")
endforeach()
file(COPY "${SOURCE_ROOT}/deps/imgui/backends/imgui_impl_opengl3_loader.h" DESTINATION "${SOURCE_STAGE}/src/imgui")
file(COPY "${SOURCE_ROOT}/deps/imnodes/imnodes.cpp" "${SOURCE_ROOT}/deps/imnodes/imnodes.h"
    "${SOURCE_ROOT}/deps/imnodes/imnodes_internal.h" DESTINATION "${SOURCE_STAGE}/src/imnodes")
foreach(file imnodes.cpp imnodes.h imnodes_internal.h)
    set(path "${SOURCE_STAGE}/src/imnodes/${file}")
    file(READ "${path}" contents)
    string(REPLACE "\r\n" "\n" contents "${contents}")
    file(WRITE "${path}" "${contents}")
endforeach()
file(READ "${SOURCE_ROOT}/cmake/patches/imnodes-legacy.patch" patch)
string(REPLACE "\r\n" "\n" patch "${patch}")
set(local_patch "${SOURCE_STAGE}/imnodes-legacy.patch")
file(WRITE "${local_patch}" "${patch}")
execute_process(COMMAND "${GIT_EXECUTABLE}" apply --no-index --whitespace=nowarn "${local_patch}"
    WORKING_DIRECTORY "${SOURCE_STAGE}/src" RESULT_VARIABLE result ERROR_VARIABLE error)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Failed to apply the legacy imnodes patch: ${error}")
endif()
file(REMOVE "${local_patch}")
foreach(component rmsd sobol)
    file(COPY "${SOURCE_ROOT}/src/${component}/" DESTINATION "${SOURCE_STAGE}/src/${component}")
    file(COPY "${SOURCE_ROOT}/include/${component}" DESTINATION "${SOURCE_STAGE}/include")
    # Keep headers beside their checked-in sources as well, so consumers can
    # select just this group without prioritizing unrelated package headers.
    file(COPY "${SOURCE_ROOT}/include/${component}/" DESTINATION "${SOURCE_STAGE}/src/${component}")
endforeach()
file(COPY "${SOURCE_ROOT}/deps/imgui/LICENSE.txt" DESTINATION "${SOURCE_STAGE}/share/licenses/imgui")
file(COPY "${SOURCE_ROOT}/deps/imnodes/LICENSE.md" DESTINATION "${SOURCE_STAGE}/share/licenses/imnodes")
file(COPY "${SOURCE_ROOT}/cmake/NuparuSourcesConfig.cmake" DESTINATION "${SOURCE_STAGE}/lib/cmake/NuparuSources")
