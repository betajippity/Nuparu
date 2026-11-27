# libzmq 4.3.5 calls project() before cmake_minimum_required(), so set this
# policy before its first compiler is enabled. The regular Nuparu arguments
# provide CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded and keep the policy NEW
# after libzmq's subsequent cmake_minimum_required() call.
cmake_policy(SET CMP0091 NEW)

# An existing libzmq build directory can have runtime options cached by the
# previous OLD-policy configuration. Remove only those options in this nested
# project; CMake's runtime-library property will supply the correct option.
foreach(language C CXX)
    foreach(config "" _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
        set(variable "CMAKE_${language}_FLAGS${config}")
        while("${${variable}}" MATCHES "(^|[ \t])[-/]M[DT]d?($|[ \t])")
            string(REGEX REPLACE "(^|[ \t])[-/]M[DT]d?($|[ \t])" " " ${variable} "${${variable}}")
        endwhile()
    endforeach()
endforeach()
