get_filename_component(_nuparu_sources_prefix "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
foreach(file src/imgui/imgui.cpp src/imgui/imgui_impl_osx.mm src/imgui/imgui_impl_metal.mm
             src/imnodes/imnodes.cpp src/rmsd/rmsd.c src/rmsd/rmsd.h src/sobol/sobol.cpp src/sobol/sobol.h
             include/rmsd/rmsd.h include/sobol/sobol.h)
    if(NOT EXISTS "${_nuparu_sources_prefix}/${file}")
        message(FATAL_ERROR "Nuparu source package is missing ${file}")
    endif()
endforeach()
set(NUPARU_PACKAGE_SOURCES_DIR "${_nuparu_sources_prefix}/src")
set(NUPARU_PACKAGE_SOURCES_INCLUDE "${_nuparu_sources_prefix}/include")
if(NOT TARGET Nuparu::sources)
    add_library(Nuparu::sources INTERFACE IMPORTED GLOBAL)
    set_target_properties(Nuparu::sources PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        "${NUPARU_PACKAGE_SOURCES_DIR};${NUPARU_PACKAGE_SOURCES_INCLUDE}")
endif()
