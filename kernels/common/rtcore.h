// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../include/embree4/rtcore.h"
RTC_NAMESPACE_USE

namespace embree
{  
  /*! decoding of intersection flags */
  __forceinline bool isCoherent  (RTCRayQueryFlags flags) { return (flags & RTC_RAY_QUERY_FLAG_COHERENT) == RTC_RAY_QUERY_FLAG_COHERENT; }
  __forceinline bool isIncoherent(RTCRayQueryFlags flags) { return (flags & RTC_RAY_QUERY_FLAG_COHERENT) == RTC_RAY_QUERY_FLAG_INCOHERENT; }

/*! Macros used in the rtcore API implementation */
#if 0
#  define RTC_CATCH_BEGIN
#  define RTC_CATCH_END(device)
#  define RTC_CATCH_END2(scene)
#  define RTC_CATCH_END2_FALSE(scene) return false;
#else
  
#define RTC_CATCH_BEGIN try {
  
#define RTC_CATCH_END(device)                                                \
  } catch (std::bad_alloc&) {                                                   \
    Device::process_error(device,RTC_ERROR_OUT_OF_MEMORY,"out of memory");      \
  } catch (rtcore_error& e) {                                                   \
    Device::process_error(device,e.error,e.what());                             \
  } catch (std::exception& e) {                                                 \
    Device::process_error(device,RTC_ERROR_UNKNOWN,e.what());                   \
  } catch (...) {                                                               \
    Device::process_error(device,RTC_ERROR_UNKNOWN,"unknown exception caught"); \
  }
  
#define RTC_CATCH_END2(scene)                                                \
  } catch (std::bad_alloc&) {                                                   \
    Device* device = scene ? scene->device : nullptr;		\
    Device::process_error(device,RTC_ERROR_OUT_OF_MEMORY,"out of memory");      \
  } catch (rtcore_error& e) {                                                   \
    Device* device = scene ? scene->device : nullptr;                           \
    Device::process_error(device,e.error,e.what());                             \
  } catch (std::exception& e) {                                                 \
    Device* device = scene ? scene->device : nullptr;                           \
    Device::process_error(device,RTC_ERROR_UNKNOWN,e.what());                   \
  } catch (...) {                                                               \
    Device* device = scene ? scene->device : nullptr;                           \
    Device::process_error(device,RTC_ERROR_UNKNOWN,"unknown exception caught"); \
  }

#define RTC_CATCH_END2_FALSE(scene)                                             \
  } catch (std::bad_alloc&) {                                                   \
    Device* device = scene ? scene->device : nullptr;                           \
    Device::process_error(device,RTC_ERROR_OUT_OF_MEMORY,"out of memory");      \
    return false;                                                               \
  } catch (rtcore_error& e) {                                                   \
    Device* device = scene ? scene->device : nullptr;                           \
    Device::process_error(device,e.error,e.what());                             \
    return false;                                                               \
  } catch (std::exception& e) {                                                 \
    Device* device = scene ? scene->device : nullptr;                           \
    Device::process_error(device,RTC_ERROR_UNKNOWN,e.what());                   \
    return false;                                                               \
  } catch (...) {                                                               \
    Device* device = scene ? scene->device : nullptr;                           \
    Device::process_error(device,RTC_ERROR_UNKNOWN,"unknown exception caught"); \
    return false;                                                               \
  }

#endif
  
#define RTC_VERIFY_HANDLE(handle)                               \
  if (handle == nullptr) {                                         \
    throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"invalid argument"); \
  }

#define RTC_VERIFY_GEOMID(id)                                   \
  if (id == RTC_INVALID_GEOMETRY_ID) {                             \
    throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"invalid argument"); \
  }

#define RTC_VERIFY_UPPER(id,upper)                              \
  if (id > upper) {                                                \
    throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"invalid argument"); \
  }

#define RTC_VERIFY_RANGE(id,lower,upper)	\
  if (id < lower || id > upper)						  \
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"argument out of bounds");
  
#if 0 // enable to debug print all API calls
#define RTC_TRACE(x) std::cout << #x << std::endl;
#else
#define RTC_TRACE(x) 
#endif

  /*! used to throw embree API errors */
  struct rtcore_error : public std::exception
  {
    __forceinline rtcore_error(RTCError error, const std::string& str)
      : error(error), str(str) {}
    
    ~rtcore_error() throw() {}
    
    const char* what () const throw () {
      return str.c_str();
    }
    
    RTCError error;
    std::string str;
  };

#if defined(DEBUG) // only report file and line in debug mode
  #define throw_RTCError(error,str) \
    throw rtcore_error(error,std::string(__FILE__) + " (" + toString(__LINE__) + "): " + std::string(str));
#else
  #define throw_RTCError(error,str) \
    throw rtcore_error(error,str);
#endif

#define RTC_BUILD_ARGUMENTS_HAS(settings,member) \
  (settings.byteSize > (offsetof(RTCBuildArguments,member)+sizeof(settings.member))) 
}
