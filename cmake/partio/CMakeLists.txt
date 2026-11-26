cmake_minimum_required(VERSION 3.20)
project(NuparuPartio VERSION 1.19.0 LANGUAGES CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Partio 1.19.0's top-level build requires Python, GLUT and its tools even
# for a library-only build. Mirror its src/lib target without those extras.
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
file(GLOB partio_sources "${PARTIO_SOURCE}/src/lib/io/*.cpp" "${PARTIO_SOURCE}/src/lib/core/*.cpp")
file(GLOB partio_headers "${PARTIO_SOURCE}/src/lib/*.h")
if(NOT partio_sources OR NOT partio_headers)
    message(FATAL_ERROR "Partio sources are missing: ${PARTIO_SOURCE}")
endif()
add_library(partio STATIC ${partio_sources})
set_target_properties(partio PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(partio PUBLIC cxx_std_14)
target_include_directories(partio PRIVATE "${PARTIO_SOURCE}/src/lib")
target_include_directories(partio PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_compile_definitions(partio PRIVATE PARTIO_USE_ZLIB)
if(WIN32)
    target_compile_definitions(partio PRIVATE PARTIO_WIN32 _USE_MATH_DEFINES)
endif()
target_link_libraries(partio PUBLIC ZLIB::ZLIB Threads::Threads)
install(TARGETS partio EXPORT PartioTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${partio_headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/partio)
install(FILES "${PARTIO_SOURCE}/LICENSE" DESTINATION share/licenses/partio)
install(EXPORT PartioTargets NAMESPACE Partio:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Partio)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/PartioConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PartioConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Partio)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/PartioConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY ExactVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/PartioConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PartioConfigVersion.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Partio)
