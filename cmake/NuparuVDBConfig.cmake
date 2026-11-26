get_filename_component(_nuparu_vdb_prefix "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
include(CMakeFindDependencyMacro)
find_dependency(NuparuBlosc CONFIG PATHS "${_nuparu_vdb_prefix}/lib/cmake/NuparuBlosc" NO_DEFAULT_PATH)
find_dependency(TBB 2022.1.0 EXACT CONFIG COMPONENTS tbb PATHS "${_nuparu_vdb_prefix}/lib/cmake/TBB" NO_DEFAULT_PATH)
find_dependency(Boost 1.92.0 EXACT CONFIG COMPONENTS iostreams PATHS "${_nuparu_vdb_prefix}/lib/cmake/Boost-1.92.0" NO_DEFAULT_PATH)
find_dependency(Imath 3.1.12 EXACT CONFIG PATHS "${_nuparu_vdb_prefix}/lib/cmake/Imath" NO_DEFAULT_PATH)
if(WIN32)
    set(_nuparu_vdb_archive "${_nuparu_vdb_prefix}/lib/libopenvdb.lib")
else()
    set(_nuparu_vdb_archive "${_nuparu_vdb_prefix}/lib/libopenvdb.a")
endif()
if(NOT TARGET Nuparu::nanovdb)
    add_library(Nuparu::nanovdb INTERFACE IMPORTED GLOBAL)
    set_target_properties(Nuparu::nanovdb PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_nuparu_vdb_prefix}/include"
        INTERFACE_COMPILE_FEATURES cxx_std_17
        INTERFACE_COMPILE_DEFINITIONS "NANOVDB_USE_BLOSC;NANOVDB_USE_ZIP;NANOVDB_USE_TBB"
        INTERFACE_LINK_LIBRARIES "Nuparu::blosc;ZLIB::ZLIB;TBB::tbb;Threads::Threads")
endif()
if(@NUPARU_NANOVDB_CUDA@ AND NOT TARGET Nuparu::nanovdb_cuda)
    # CUDA users find CUDAToolkit before linking this optional target.
    add_library(Nuparu::nanovdb_cuda INTERFACE IMPORTED GLOBAL)
    set_target_properties(Nuparu::nanovdb_cuda PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS NANOVDB_USE_CUDA
        INTERFACE_LINK_LIBRARIES "Nuparu::nanovdb;CUDA::cudart")
endif()
if(NOT EXISTS "${_nuparu_vdb_archive}" OR NOT EXISTS "${_nuparu_vdb_prefix}/include/openvdb/version.h")
    message(FATAL_ERROR "Nuparu OpenVDB package is incomplete: ${_nuparu_vdb_prefix}")
endif()
if(NOT TARGET Nuparu::openvdb)
    add_library(Nuparu::openvdb STATIC IMPORTED GLOBAL)
    set_target_properties(Nuparu::openvdb PROPERTIES IMPORTED_LOCATION "${_nuparu_vdb_archive}"
        INTERFACE_INCLUDE_DIRECTORIES "${_nuparu_vdb_prefix}/include"
        INTERFACE_COMPILE_DEFINITIONS "OPENVDB_STATICLIB;BOOST_ALL_NO_LIB"
        INTERFACE_COMPILE_FEATURES cxx_std_17
        INTERFACE_LINK_LIBRARIES "Imath::Imath;TBB::tbb;Nuparu::blosc;ZLIB::ZLIB;Threads::Threads;Boost::iostreams")
endif()
