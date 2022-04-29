## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

SET(EMBREE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/@EMBREE_RELATIVE_ROOT_DIR@")
GET_FILENAME_COMPONENT(EMBREE_ROOT_DIR "${EMBREE_ROOT_DIR}" ABSOLUTE)

SET(EMBREE_INCLUDE_DIRS "${EMBREE_ROOT_DIR}/@CMAKE_INSTALL_INCLUDEDIR@")
SET(EMBREE_LIBRARY "${EMBREE_ROOT_DIR}/@CMAKE_INSTALL_LIBDIR@/@EMBREE_LIBRARY_FULLNAME@")
SET(EMBREE_LIBRARIES ${EMBREE_LIBRARY})

SET(EMBREE_VERSION @EMBREE_VERSION@)
SET(EMBREE_VERSION_MAJOR @EMBREE_VERSION_MAJOR@)
SET(EMBREE_VERSION_MINOR @EMBREE_VERSION_MINOR@)
SET(EMBREE_VERSION_PATCH @EMBREE_VERSION_PATCH@)
SET(EMBREE_VERSION_NOTE "@EMBREE_VERSION_NOTE@")

SET(EMBREE_MAX_ISA @EMBREE_MAX_ISA@)
SET(EMBREE_ISA_SSE2  @EMBREE_ISA_SSE2@)
SET(EMBREE_ISA_SSE42 @EMBREE_ISA_SSE42@)
SET(EMBREE_ISA_AVX @EMBREE_ISA_AVX@) 
SET(EMBREE_ISA_AVX2  @EMBREE_ISA_AVX2@)
SET(EMBREE_ISA_AVX512 @EMBREE_ISA_AVX512@)
SET(EMBREE_ISA_AVX512SKX @EMBREE_ISA_AVX512@) # just for compatibility
SET(EMBREE_ISA_NEON @EMBREE_ISA_NEON@)

SET(EMBREE_BUILD_TYPE @CMAKE_BUILD_TYPE@)
SET(EMBREE_ISPC_SUPPORT @EMBREE_ISPC_SUPPORT@)
SET(EMBREE_STATIC_LIB @EMBREE_STATIC_LIB@)
SET(EMBREE_DPCPP_SUPPORT @EMBREE_DPCPP_SUPPORT@)
SET(EMBREE_TUTORIALS @EMBREE_TUTORIALS@)

SET(EMBREE_RAY_MASK @EMBREE_RAY_MASK@)
SET(EMBREE_STAT_COUNTERS @EMBREE_STAT_COUNTERS@)
SET(EMBREE_BACKFACE_CULLING @EMBREE_BACKFACE_CULLING@)
SET(EMBREE_FILTER_FUNCTION @EMBREE_FILTER_FUNCTION@)
SET(EMBREE_IGNORE_INVALID_RAYS @EMBREE_IGNORE_INVALID_RAYS@)
SET(EMBREE_TASKING_SYSTEM @EMBREE_TASKING_SYSTEM@)
SET(EMBREE_COMPACT_POLYS @EMBREE_COMPACT_POLYS@)

SET(EMBREE_GEOMETRY_TRIANGLE @EMBREE_GEOMETRY_TRIANGLE@)
SET(EMBREE_GEOMETRY_QUAD @EMBREE_GEOMETRY_QUAD@)
SET(EMBREE_GEOMETRY_CURVE @EMBREE_GEOMETRY_CURVE@)
SET(EMBREE_GEOMETRY_SUBDIVISION @EMBREE_GEOMETRY_SUBDIVISION@)
SET(EMBREE_GEOMETRY_USER @EMBREE_GEOMETRY_USER@)
SET(EMBREE_GEOMETRY_POINT @EMBREE_GEOMETRY_POINT@)

SET(EMBREE_RAY_PACKETS @EMBREE_RAY_PACKETS@)
SET(EMBREE_MAX_INSTANCE_LEVEL_COUNT @EMBREE_MAX_INSTANCE_LEVEL_COUNT@)
SET(EMBREE_CURVE_SELF_INTERSECTION_AVOIDANCE_FACTOR @EMBREE_CURVE_SELF_INTERSECTION_AVOIDANCE_FACTOR@)
SET(EMBREE_MIN_WIDTH @EMBREE_MIN_WIDTH@)

IF (EMBREE_STATIC_LIB)
  INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/sys-targets.cmake")
  INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/math-targets.cmake")
  INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/simd-targets.cmake")
  INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/lexers-targets.cmake")
  INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/TBB-targets.cmake")
  INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/tasking-targets.cmake")

  IF (EMBREE_DPCPP_SUPPORT)
    INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/rthwif-targets.cmake")
    INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/embree_sycl-targets.cmake")
  ENDIF()

  IF (EMBREE_ISA_SSE42)
    INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/embree_sse42-targets.cmake")
  ENDIF()

  IF (EMBREE_ISA_AVX)
    INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/embree_avx-targets.cmake")
  ENDIF()

  IF (EMBREE_ISA_AVX2)
    INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/embree_avx2-targets.cmake")
  ENDIF()

  IF (EMBREE_ISA_AVX512)
    INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/embree_avx512-targets.cmake")
  ENDIF()

ENDIF()

INCLUDE("${EMBREE_ROOT_DIR}/@EMBREE_CMAKEEXPORT_DIR@/embree-targets.cmake")
