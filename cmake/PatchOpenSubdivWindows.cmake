# OpenSubdiv 3.8.0's legacy option passes /MT directly to nvcc. Nuparu uses
# CMAKE_MSVC_RUNTIME_LIBRARY instead, which handles C, C++, and CUDA. Keep the
# option enabled to avoid its alternative /NODEFAULTLIB:libcmt.lib flags.
function(nuparu_patch_opensubdiv_windows cmake_file)
    file(READ "${cmake_file}" contents)
    string(REPLACE "\r\n" "\n" contents "${contents}")
    set(original [=[        add_compile_options(
            "$<$<CONFIG:Debug>:/MTd>"
            "$<$<CONFIG:RelWithDebInfo>:/MT>"
            "$<$<CONFIG:Release>:/MT>"
            "$<$<CONFIG:MinSizeRel>:/MT>"
        )]=])
    set(replacement "        # MSVC runtime is selected by CMAKE_MSVC_RUNTIME_LIBRARY.")
    string(FIND "${contents}" "${original}" original_position)
    string(FIND "${contents}" "${replacement}" replacement_position)
    if(NOT original_position EQUAL -1)
        string(REPLACE "${original}" "${replacement}" contents "${contents}")
        file(WRITE "${cmake_file}" "${contents}")
    elseif(replacement_position EQUAL -1)
        message(FATAL_ERROR "Cannot patch OpenSubdiv's MSVC runtime options; check the upstream revision")
    endif()
endfunction()
