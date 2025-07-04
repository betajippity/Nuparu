// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#if !defined(EMBREE_SYCL_SUPPORT)
/* #undef EMBREE_SYCL_SUPPORT */
#endif

#define RTC_VERSION_MAJOR 4
#define RTC_VERSION_MINOR 4
#define RTC_VERSION_PATCH 0
#define RTC_VERSION 40400
#define RTC_VERSION_STRING "4.4.0"

#define RTC_MAX_INSTANCE_LEVEL_COUNT 16

#define EMBREE_GEOMETRY_INSTANCE_ARRAY
#if defined(EMBREE_GEOMETRY_INSTANCE_ARRAY)
  #define RTC_GEOMETRY_INSTANCE_ARRAY
#endif

#define EMBREE_SYCL_GEOMETRY_CALLBACK 0

#define EMBREE_MIN_WIDTH 0
#define RTC_MIN_WIDTH EMBREE_MIN_WIDTH

#if !defined(EMBREE_STATIC_LIB)
#define EMBREE_STATIC_LIB
#endif
/* #undef EMBREE_API_NAMESPACE */

#if defined(EMBREE_API_NAMESPACE)
#  define RTC_NAMESPACE 
#  define RTC_NAMESPACE_BEGIN namespace  {
#  define RTC_NAMESPACE_END }
#  define RTC_NAMESPACE_USE using namespace ;
#  define RTC_API_EXTERN_C
#  define RTC_API_EXTERN_CPP
#  undef EMBREE_API_NAMESPACE
#else
#  define RTC_NAMESPACE_BEGIN
#  define RTC_NAMESPACE_END
#  define RTC_NAMESPACE_USE
#  if defined(__cplusplus)
#    define RTC_API_EXTERN_C extern "C"
#    define RTC_API_EXTERN_CPP extern "C++"
#  else
#    define RTC_API_EXTERN_C
#  endif
#endif

#if defined(ISPC)
#  define RTC_API_IMPORT extern "C" unmasked
#  define RTC_API_EXPORT extern "C" unmasked
#elif defined(EMBREE_STATIC_LIB)
#  define RTC_API_IMPORT RTC_API_EXTERN_C
#  define RTC_API_EXPORT RTC_API_EXTERN_C
#elif defined(_WIN32)
#  define RTC_API_IMPORT RTC_API_EXTERN_C __declspec(dllimport)
#  define RTC_API_EXPORT RTC_API_EXTERN_C __declspec(dllexport)
#else
#  define RTC_API_IMPORT RTC_API_EXTERN_C
#  define RTC_API_EXPORT RTC_API_EXTERN_C __attribute__ ((visibility ("default")))
#endif

#if defined(ISPC)
#  define RTC_API_IMPORT_CPP extern "C++" unmasked
#  define RTC_API_EXPORT_CPP extern "C++" unmasked
#elif defined(EMBREE_STATIC_LIB)
#  define RTC_API_IMPORT_CPP RTC_API_EXTERN_CPP
#  define RTC_API_EXPORT_CPP RTC_API_EXTERN_CPP
#elif defined(_WIN32)
#  define RTC_API_IMPORT_CPP RTC_API_EXTERN_CPP __declspec(dllimport)
#  define RTC_API_EXPORT_CPP RTC_API_EXTERN_CPP __declspec(dllexport)
#else
#  define RTC_API_IMPORT_CPP RTC_API_EXTERN_CPP
#  define RTC_API_EXPORT_CPP RTC_API_EXTERN_CPP __attribute__ ((visibility ("default")))
#endif

#if defined(RTC_EXPORT_API)
#  define RTC_API RTC_API_EXPORT
#else
#  define RTC_API RTC_API_IMPORT
#endif

#if defined(RTC_EXPORT_API)
#  define RTC_API_CPP RTC_API_EXPORT_CPP
#else
#  define RTC_API_CPP RTC_API_IMPORT_CPP
#endif

#if defined(ISPC)
#  define RTC_SYCL_INDIRECTLY_CALLABLE
#elif defined(__SYCL_DEVICE_ONLY__)
#  define RTC_SYCL_INDIRECTLY_CALLABLE [[intel::device_indirectly_callable]] SYCL_EXTERNAL
#  define RTC_SYCL_API                 SYCL_EXTERNAL
#else
#  define RTC_SYCL_INDIRECTLY_CALLABLE
#  define RTC_SYCL_API RTC_API
#endif

