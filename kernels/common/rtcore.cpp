// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#ifdef _WIN32
#  define RTC_API extern "C" __declspec(dllexport)
#else
#  define RTC_API extern "C" __attribute__ ((visibility ("default")))
#endif

#include "default.h"
#include "device.h"
#include "scene.h"
#include "context.h"
#include "../../include/embree3/rtcore_ray.h"

namespace embree
{  
  /* mutex to make API thread safe */
  static MutexSys g_mutex;

  RTC_API RTCDevice rtcNewDevice(const char* cfg)
  {
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewDevice);
    Lock<MutexSys> lock(g_mutex);
    Device* device = new Device(cfg,false);
    return (RTCDevice) device->refInc();
    RTC_CATCH_END(nullptr);
    return (RTCDevice) nullptr;
  }

  RTC_API void rtcRetainDevice(RTCDevice hdevice) 
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcRetainDevice);
    RTC_VERIFY_HANDLE(hdevice);
    Lock<MutexSys> lock(g_mutex);
    device->refInc();
    RTC_CATCH_END(nullptr);
  }
  
  RTC_API void rtcReleaseDevice(RTCDevice hdevice) 
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcReleaseDevice);
    RTC_VERIFY_HANDLE(hdevice);
    Lock<MutexSys> lock(g_mutex);
    device->refDec();
    RTC_CATCH_END(nullptr);
  }
  
  RTC_API void rtcSetDeviceProperty(RTCDevice hdevice, const RTCDeviceProperty prop, ssize_t val)
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetDeviceProperty);
    const bool internal_prop = (size_t)prop >= 1000000 && (size_t)prop < 1000004;
    if (!internal_prop) RTC_VERIFY_HANDLE(hdevice); // allow NULL device for special internal settings
    Lock<MutexSys> lock(g_mutex);
    device->setProperty(prop,val);
    RTC_CATCH_END(device);
  }

  RTC_API ssize_t rtcGetDeviceProperty(RTCDevice hdevice, const RTCDeviceProperty prop)
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetDeviceProperty);
    RTC_VERIFY_HANDLE(hdevice);
    Lock<MutexSys> lock(g_mutex);
    return device->getProperty(prop);
    RTC_CATCH_END(device);
    return 0;
  }

  RTC_API RTCError rtcGetDeviceError(RTCDevice hdevice)
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetDeviceError);
    if (device == nullptr) return Device::getThreadErrorCode();
    else                   return device->getDeviceErrorCode();
    RTC_CATCH_END(device);
    return RTC_ERROR_UNKNOWN;
  }

  RTC_API void rtcSetDeviceErrorFunction(RTCDevice hdevice, RTCErrorFunction func, void* userPtr) 
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetDeviceErrorFunction);
    RTC_VERIFY_HANDLE(hdevice);
    device->setErrorFunction(func,userPtr);
    RTC_CATCH_END(device);
  }

  RTC_API void rtcSetDeviceMemoryMonitorFunction(RTCDevice hdevice, RTCMemoryMonitorFunction func, void* userPtr) 
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetDeviceMemoryMonitorFunction);
    device->setMemoryMonitorFunction(func,userPtr);
    RTC_CATCH_END(device);
  }

  RTC_API RTCBuffer rtcNewBuffer(RTCDevice hdevice, size_t byteSize)
  {
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewBuffer);
    RTC_VERIFY_HANDLE(hdevice);
    Buffer* buffer = new Buffer((Device*)hdevice, byteSize);
    return (RTCBuffer)buffer->refInc();
    RTC_CATCH_END((Device*)hdevice);
    return nullptr;
  }

  RTC_API RTCBuffer rtcNewSharedBuffer(RTCDevice hdevice, void* ptr, size_t byteSize)
  {
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewSharedBuffer);
    RTC_VERIFY_HANDLE(hdevice);
    Buffer* buffer = new Buffer((Device*)hdevice, byteSize, ptr);
    return (RTCBuffer)buffer->refInc();
    RTC_CATCH_END((Device*)hdevice);
    return nullptr;
  }

  RTC_API void* rtcGetBufferData(RTCBuffer hbuffer)
  {
    Buffer* buffer = (Buffer*)hbuffer;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetBufferData);
    RTC_VERIFY_HANDLE(hbuffer);
    return buffer->data();
    RTC_CATCH_END2(buffer);
    return nullptr;
  }

  RTC_API void rtcRetainBuffer(RTCBuffer hbuffer)
  {
    Buffer* buffer = (Buffer*)hbuffer;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcRetainBuffer);
    RTC_VERIFY_HANDLE(hbuffer);
    buffer->refInc();
    RTC_CATCH_END2(buffer);
  }
  
  RTC_API void rtcReleaseBuffer(RTCBuffer hbuffer)
  {
    Buffer* buffer = (Buffer*)hbuffer;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcReleaseBuffer);
    RTC_VERIFY_HANDLE(hbuffer);
    buffer->refDec();
    RTC_CATCH_END2(buffer);
  }

  RTC_API RTCScene rtcNewScene (RTCDevice hdevice) 
  {
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewScene);
    RTC_VERIFY_HANDLE(hdevice);
    Scene* scene = new Scene((Device*)hdevice);
    return (RTCScene) scene->refInc();
    RTC_CATCH_END((Device*)hdevice);
    return nullptr;
  }

  RTC_API void rtcSetSceneProgressMonitorFunction(RTCScene hscene, RTCProgressMonitorFunction func, void* ptr) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetSceneProgressMonitorFunction);
    RTC_VERIFY_HANDLE(hscene);
    scene->setProgressMonitorFunction(func,ptr);
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcSetSceneBuildQuality (RTCScene hscene, RTCBuildQuality quality) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetSceneBuildQuality);
    RTC_VERIFY_HANDLE(hscene);
    if (quality != RTC_BUILD_QUALITY_LOW &&
        quality != RTC_BUILD_QUALITY_MEDIUM &&
        quality != RTC_BUILD_QUALITY_HIGH)
      throw std::runtime_error("invalid build quality");
    scene->setBuildQuality(quality);
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcSetSceneFlags (RTCScene hscene, RTCSceneFlags sflags) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetSceneFlags);
    RTC_VERIFY_HANDLE(hscene);
    scene->setSceneFlags(sflags);
    RTC_CATCH_END2(scene);
  }

  RTC_API RTCSceneFlags rtcGetSceneFlags(RTCScene hscene)
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetSceneFlags);
    RTC_VERIFY_HANDLE(hscene);
    return scene->getSceneFlags();
    RTC_CATCH_END2(scene);
    return RTC_SCENE_FLAG_NONE;
  }
  
  RTC_API void rtcCommitScene (RTCScene hscene) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcCommitScene);
    RTC_VERIFY_HANDLE(hscene);
    scene->commit(false);
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcJoinCommitScene (RTCScene hscene) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcJoinCommitScene);
    RTC_VERIFY_HANDLE(hscene);
    scene->commit(true);
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcGetSceneBounds(RTCScene hscene, RTCBounds* bounds_o)
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetSceneBounds);
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    BBox3fa bounds = scene->bounds.bounds();
    bounds_o->lower_x = bounds.lower.x;
    bounds_o->lower_y = bounds.lower.y;
    bounds_o->lower_z = bounds.lower.z;
    bounds_o->align0  = 0;
    bounds_o->upper_x = bounds.upper.x;
    bounds_o->upper_y = bounds.upper.y;
    bounds_o->upper_z = bounds.upper.z;
    bounds_o->align1  = 0;
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcGetSceneLinearBounds(RTCScene hscene, RTCLinearBounds* bounds_o)
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetSceneBounds);
    RTC_VERIFY_HANDLE(hscene);
    if (bounds_o == nullptr)
      throw_RTCError(RTC_ERROR_INVALID_OPERATION,"invalid destination pointer");
    if (scene->isModified())
      throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    
    bounds_o->bounds0.lower_x = scene->bounds.bounds0.lower.x;
    bounds_o->bounds0.lower_y = scene->bounds.bounds0.lower.y;
    bounds_o->bounds0.lower_z = scene->bounds.bounds0.lower.z;
    bounds_o->bounds0.align0  = 0;
    bounds_o->bounds0.upper_x = scene->bounds.bounds0.upper.x;
    bounds_o->bounds0.upper_y = scene->bounds.bounds0.upper.y;
    bounds_o->bounds0.upper_z = scene->bounds.bounds0.upper.z;
    bounds_o->bounds0.align1  = 0;
    bounds_o->bounds1.lower_x = scene->bounds.bounds1.lower.x;
    bounds_o->bounds1.lower_y = scene->bounds.bounds1.lower.y;
    bounds_o->bounds1.lower_z = scene->bounds.bounds1.lower.z;
    bounds_o->bounds1.align0  = 0;
    bounds_o->bounds1.upper_x = scene->bounds.bounds1.upper.x;
    bounds_o->bounds1.upper_y = scene->bounds.bounds1.upper.y;
    bounds_o->bounds1.upper_z = scene->bounds.bounds1.upper.z;
    bounds_o->bounds1.align1  = 0;
    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcIntersect1 (RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersect1);
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)ray) & 0x0F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT3(normal.travs,1,1,1);
    IntersectContext context(scene,user_context);
    scene->intersectors.intersect(*ray,&context);
#if defined(DEBUG)
    ((Ray*)ray)->verifyHit();
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcIntersect4 (const int* valid, RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit4* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersect4);

#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x0F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "mask not aligned to 16 bytes");   
    if (((size_t)ray)   & 0x0F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<4; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,cnt,cnt,cnt);

    IntersectContext context(scene,user_context);
#if !defined(EMBREE_RAY_PACKETS)
    Ray4* ray4 = (Ray4*) ray;
    for (size_t i=0; i<4; i++) {
      if (!valid[i]) continue;
      Ray ray1; ray4->get(i,ray1);
      scene->intersectors.intersect((RTCRayHit&)ray1,&context);
      ray4->set(i,ray1);
    }
#else
    scene->intersectors.intersect4(valid,*ray,&context);
#endif
    
    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcIntersect8 (const int* valid, RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit8* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersect8);

#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x1F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "mask not aligned to 32 bytes");   
    if (((size_t)ray)   & 0x1F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 32 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<8; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,cnt,cnt,cnt);

    IntersectContext context(scene,user_context);
#if !defined(EMBREE_RAY_PACKETS)
    Ray8* ray8 = (Ray8*) ray;
    for (size_t i=0; i<8; i++) {
      if (!valid[i]) continue;
      Ray ray1; ray8->get(i,ray1);
      scene->intersectors.intersect((RTCRayHit&)ray1,&context);
      ray8->set(i,ray1);
    }
#else
    if (likely(scene->intersectors.intersector8))
      scene->intersectors.intersect8(valid,*ray,&context);
    else
      scene->device->rayStreamFilters.filterSOA(scene,(char*)ray,8,1,sizeof(RTCRayHit8),&context,true);
#endif
    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcIntersect16 (const int* valid, RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit16* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersect16);

#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x3F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "mask not aligned to 64 bytes");   
    if (((size_t)ray)   & 0x3F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 64 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<16; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,cnt,cnt,cnt);

    IntersectContext context(scene,user_context);
#if !defined(EMBREE_RAY_PACKETS)
    Ray16* ray16 = (Ray16*) ray;
    for (size_t i=0; i<16; i++) {
      if (!valid[i]) continue;
      Ray ray1; ray16->get(i,ray1);
      scene->intersectors.intersect((RTCRayHit&)ray1,&context);
      ray16->set(i,ray1);
    }
#else
    if (likely(scene->intersectors.intersector16))
      scene->intersectors.intersect16(valid,*ray,&context);
    else
      scene->device->rayStreamFilters.filterSOA(scene,(char*)ray,16,1,sizeof(RTCRayHit16),&context,true);
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcIntersect1M (RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit* rh, const unsigned int M, const size_t stride) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersect1M);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rh ) & 0x03) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,M,M,M);
    IntersectContext context(scene,user_context);

    /* fast codepath for single rays */
    if (likely(M == 1)) {
      if (likely(rh->ray.tnear <= rh->ray.tfar)) 
        scene->intersectors.intersect(*rh,&context);
    } 

    /* codepath for streams */
    else {
      scene->device->rayStreamFilters.filterAOS(scene,rh,M,stride,&context,true);   
    }
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcIntersect1M not supported");
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcIntersect1Mp (RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit** rn, const unsigned int M) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersect1Mp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rn) & 0x03) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,M,M,M);
    IntersectContext context(scene,user_context);

    /* fast codepath for single rays */
    if (likely(M == 1)) {
      if (likely(rn[0]->ray.tnear <= rn[0]->ray.tfar)) 
        scene->intersectors.intersect(*rn[0],&context);
    } 

    /* codepath for streams */
    else {
      scene->device->rayStreamFilters.filterAOP(scene,rn,M,&context,true);   
    }
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcIntersect1Mp not supported");
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcIntersectNM (RTCScene hscene, RTCIntersectContext* user_context, struct RTCRayHitN* rh, const unsigned int N, const unsigned int M, const size_t stride) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersectNM);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rh) & 0x03) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,N*M,N*M,N*M);
    IntersectContext context(scene,user_context);

    /* code path for single ray streams */
    if (likely(N == 1))
    {
      /* fast code path for streams of size 1 */
      if (likely(M == 1)) {
        if (likely(((RTCRayHit*)rh)->ray.tnear <= ((RTCRayHit*)rh)->ray.tfar))
          scene->intersectors.intersect(*(RTCRayHit*)rh,&context);
      } 
      /* normal codepath for single ray streams */
      else {
        scene->device->rayStreamFilters.filterAOS(scene,(RTCRayHit*)rh,M,stride,&context,true);
      }
    }
    /* code path for ray packet streams */
    else {
      scene->device->rayStreamFilters.filterSOA(scene,(char*)rh,N,M,stride,&context,true);
    }
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcIntersectNM not supported");
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcIntersectNp (RTCScene hscene, RTCIntersectContext* user_context, const RTCRayHitNp* rh, const unsigned int N) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcIntersectNp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rh->ray.org_x ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.org_x not aligned to 4 bytes");   
    if (((size_t)rh->ray.org_y ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.org_y not aligned to 4 bytes");   
    if (((size_t)rh->ray.org_z ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.org_z not aligned to 4 bytes");   
    if (((size_t)rh->ray.dir_x ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_x not aligned to 4 bytes");   
    if (((size_t)rh->ray.dir_y ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_y not aligned to 4 bytes");   
    if (((size_t)rh->ray.dir_z ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_z not aligned to 4 bytes");   
    if (((size_t)rh->ray.tnear ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_x not aligned to 4 bytes");   
    if (((size_t)rh->ray.tfar  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.tnear not aligned to 4 bytes");   
    if (((size_t)rh->ray.time  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.time not aligned to 4 bytes");   
    if (((size_t)rh->ray.mask  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.mask not aligned to 4 bytes");   
    if (((size_t)rh->hit.Ng_x  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.Ng_x not aligned to 4 bytes");   
    if (((size_t)rh->hit.Ng_y  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.Ng_y not aligned to 4 bytes");   
    if (((size_t)rh->hit.Ng_z  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.Ng_z not aligned to 4 bytes");   
    if (((size_t)rh->hit.u     ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.u not aligned to 4 bytes");   
    if (((size_t)rh->hit.v     ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.v not aligned to 4 bytes");   
    if (((size_t)rh->hit.geomID) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.geomID not aligned to 4 bytes");   
    if (((size_t)rh->hit.primID) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.primID not aligned to 4 bytes");   
    if (((size_t)rh->hit.instID) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.instID not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,N,N,N);
    IntersectContext context(scene,user_context);
    scene->device->rayStreamFilters.filterSOP(scene,*rh,N,&context,true);
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcIntersectNp not supported");
#endif
    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcOccluded1 (RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccluded1);
    STAT3(shadow.travs,1,1,1);
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)ray) & 0x0F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    IntersectContext context(scene,user_context);
    scene->intersectors.occluded(*ray,&context);
    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcOccluded4 (const int* valid, RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit4* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccluded4);

#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x0F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "mask not aligned to 16 bytes");   
    if (((size_t)ray)   & 0x0F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<4; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,cnt,cnt,cnt);

    IntersectContext context(scene,user_context);
#if !defined(EMBREE_RAY_PACKETS)
    Ray4* ray4 = (Ray4*) ray;
    for (size_t i=0; i<4; i++) {
      if (!valid[i]) continue;
      Ray ray1; ray4->get(i,ray1);
      scene->intersectors.occluded((RTCRayHit&)ray1,&context);
      ray4->geomID[i] = ray1.geomID; 
    }
#else
    scene->intersectors.occluded4(valid,*ray,&context);
#endif
    
    RTC_CATCH_END2(scene);
  }
 
  RTC_API void rtcOccluded8 (const int* valid, RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit8* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccluded8);

#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x1F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "mask not aligned to 32 bytes");   
    if (((size_t)ray)   & 0x1F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 32 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<8; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,cnt,cnt,cnt);

    IntersectContext context(scene,user_context);
#if !defined(EMBREE_RAY_PACKETS)
    Ray8* ray8 = (Ray8*) ray;
    for (size_t i=0; i<8; i++) {
      if (!valid[i]) continue;
      Ray ray1; ray8->get(i,ray1);
      scene->intersectors.occluded((RTCRayHit&)ray1,&context);
      ray8->set(i,ray1);
    }
#else
    if (likely(scene->intersectors.intersector8))
      scene->intersectors.occluded8(valid,*ray,&context);
    else
      scene->device->rayStreamFilters.filterSOA(scene,(char*)ray,8,1,sizeof(RTCRayHit8),&context,false);
#endif

    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcOccluded16 (const int* valid, RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit16* ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccluded16);

#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x3F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "mask not aligned to 64 bytes");   
    if (((size_t)ray)   & 0x3F) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 64 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<16; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,cnt,cnt,cnt);

    IntersectContext context(scene,user_context);
#if !defined(EMBREE_RAY_PACKETS)
    Ray16* ray16 = (Ray16*) ray;
    for (size_t i=0; i<16; i++) {
      if (!valid[i]) continue;
      Ray ray1; ray16->get(i,ray1);
      scene->intersectors.occluded((RTCRayHit&)ray1,&context);
      ray16->set(i,ray1);
    }
#else
    if (likely(scene->intersectors.intersector16))
      scene->intersectors.occluded16(valid,*ray,&context);
    else
      scene->device->rayStreamFilters.filterSOA(scene,(char*)ray,16,1,sizeof(RTCRayHit16),&context,false);
#endif

    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcOccluded1M(RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit* rh, const unsigned int M, const size_t stride) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccluded1M);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rh) & 0x03) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,M,M,M);
    IntersectContext context(scene,user_context);
    /* fast codepath for streams of size 1 */
    if (likely(M == 1)) {
      if (likely(rh->ray.tnear <= rh->ray.tfar)) 
        scene->intersectors.occluded (*rh,&context);
    } 
    /* codepath for normal streams */
    else {
      scene->device->rayStreamFilters.filterAOS(scene,rh,M,stride,&context,false);
    }
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcOccluded1M not supported");
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcOccluded1Mp(RTCScene hscene, RTCIntersectContext* user_context, RTCRayHit** rh, const unsigned int M) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccluded1Mp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rh) & 0x03) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,M,M,M);
    IntersectContext context(scene,user_context);

    /* fast codepath for streams of size 1 */
    if (likely(M == 1)) {
      if (likely(rh[0]->ray.tnear <= rh[0]->ray.tfar)) 
        scene->intersectors.occluded (*rh[0],&context);
    } 
    /* codepath for normal streams */
    else {
      scene->device->rayStreamFilters.filterAOP(scene,rh,M,&context,false);
    }
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcOccluded1Mp not supported");
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcOccludedNM(RTCScene hscene, RTCIntersectContext* user_context, RTCRayHitN* rh, const unsigned int N, const unsigned int M, const size_t stride)
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccludedNM);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (stride < sizeof(RTCRayHit)) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"stride too small");
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rh) & 0x03) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,N*M,N*N,N*N);
    IntersectContext context(scene,user_context);

    /* codepath for single rays */
    if (likely(N == 1))
    {
      /* fast path for streams of size 1 */
      if (likely(M == 1)) {
        if (likely(((RTCRayHit*)rh)->ray.tnear <= ((RTCRayHit*)rh)->ray.tfar))
          scene->intersectors.occluded (*(RTCRayHit*)rh,&context);
      } 
      /* codepath for normal ray streams */
      else {
        scene->device->rayStreamFilters.filterAOS(scene,(RTCRayHit*)rh,M,stride,&context,false);
      }
    }
    /* code path for ray packet streams */
    else {
      scene->device->rayStreamFilters.filterSOA(scene,(char*)rh,N,M,stride,&context,false);
    }
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcOccludedNM not supported");
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcOccludedNp(RTCScene hscene, RTCIntersectContext* user_context, const RTCRayHitNp* rh, const unsigned int N)
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcOccludedNp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_ERROR_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rh->ray.org_x ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.org_x not aligned to 4 bytes");   
    if (((size_t)rh->ray.org_y ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.org_y not aligned to 4 bytes");   
    if (((size_t)rh->ray.org_z ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.org_z not aligned to 4 bytes");   
    if (((size_t)rh->ray.dir_x ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_x not aligned to 4 bytes");   
    if (((size_t)rh->ray.dir_y ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_y not aligned to 4 bytes");   
    if (((size_t)rh->ray.dir_z ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_z not aligned to 4 bytes");   
    if (((size_t)rh->ray.tnear ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.dir_x not aligned to 4 bytes");   
    if (((size_t)rh->ray.tfar  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.tnear not aligned to 4 bytes");   
    if (((size_t)rh->ray.time  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.time not aligned to 4 bytes");   
    if (((size_t)rh->ray.mask  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->ray.mask not aligned to 4 bytes");   
    if (((size_t)rh->hit.Ng_x  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.Ng_x not aligned to 4 bytes");   
    if (((size_t)rh->hit.Ng_y  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.Ng_y not aligned to 4 bytes");   
    if (((size_t)rh->hit.Ng_z  ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.Ng_z not aligned to 4 bytes");   
    if (((size_t)rh->hit.u     ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.u not aligned to 4 bytes");   
    if (((size_t)rh->hit.v     ) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.v not aligned to 4 bytes");   
    if (((size_t)rh->hit.geomID) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.geomID not aligned to 4 bytes");   
    if (((size_t)rh->hit.primID) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.primID not aligned to 4 bytes");   
    if (((size_t)rh->hit.instID) & 0x03 ) throw_RTCError(RTC_ERROR_INVALID_ARGUMENT, "rh->hit.instID not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,N,N,N);
    IntersectContext context(scene,user_context);
    scene->device->rayStreamFilters.filterSOP(scene,*rh,N,&context,false);
#else
    throw_RTCError(RTC_ERROR_INVALID_OPERATION,"rtcOccludedNp not supported");
#endif
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcRetainScene (RTCScene hscene) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcRetainScene);
    RTC_VERIFY_HANDLE(hscene);
    scene->refInc();
    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcReleaseScene (RTCScene hscene) 
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcReleaseScene);
    RTC_VERIFY_HANDLE(hscene);
    scene->refDec();
    RTC_CATCH_END2(scene);
  }

  RTC_API RTCGeometry rtcNewInstance (RTCDevice hdevice, RTCScene hsource, unsigned int numTimeSteps)
  {
    Device* device = (Device*) hdevice;
    Scene* source = (Scene*) hsource;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewInstance);
    RTC_VERIFY_HANDLE(hdevice);
    RTC_VERIFY_HANDLE(hsource);
#if defined(EMBREE_GEOMETRY_USER)
    Geometry* geom = new Instance(device,source,numTimeSteps);
    return (RTCGeometry) geom->refInc();
#else
    throw_RTCError(RTC_ERROR_UNKNOWN,"rtcNewInstance is not supported");
#endif
    RTC_CATCH_END(device);
    return nullptr;
  }

  RTC_API RTCGeometry rtcNewGeometryInstance (RTCDevice hdevice,  RTCScene hscene, unsigned geomID) 
  {
    Device* device = (Device*) hdevice;
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewGeometryInstance);
    RTC_VERIFY_HANDLE(hdevice);
    RTC_VERIFY_HANDLE(hscene);
    RTC_VERIFY_GEOMID(geomID);
    Geometry* geom = new GeometryInstance(device,scene->get_locked(geomID));
    return (RTCGeometry) geom->refInc();
    RTC_CATCH_END(device);
    return nullptr;
  }

  RTC_API RTCGeometry rtcNewGeometryGroup (RTCDevice hdevice, RTCScene hscene, unsigned* geomIDs, unsigned int N)
  {
    Device* device = (Device*) hdevice;
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewGeometryGroup);
    RTC_VERIFY_HANDLE(hdevice);
    RTC_VERIFY_HANDLE(hscene);
    if (N) RTC_VERIFY_HANDLE(geomIDs);

    std::vector<Ref<Geometry>> geometries(N);
    for (size_t i=0; i<N; i++) {
      RTC_VERIFY_GEOMID(geomIDs[i]);
      geometries[i] = scene->get_locked(geomIDs[i]);
      if (geometries[i]->getType() == Geometry::GROUP)
        throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"geometry groups cannot contain other geometry groups");
      if (geometries[i]->getType() != geometries[0]->getType())
        throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"geometries inside group have to be of same type");
    }
    Geometry* geom = new GeometryGroup(device,geometries);
    return (RTCGeometry) geom->refInc();
    RTC_CATCH_END(device);
    return nullptr;
  }

  RTC_API void rtcSetGeometryInstancedScene(RTCGeometry hgeometry, RTCScene hscene)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    Ref<Scene> scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryInstancedScene);
    RTC_VERIFY_HANDLE(hgeometry);
    RTC_VERIFY_HANDLE(hscene);
    geometry->setInstancedScene(scene);
    RTC_CATCH_END2(geometry);
  }

  AffineSpace3fa loadTransform(RTCFormat format, const float* xfm)
  {
    AffineSpace3fa space = one;
    switch (format)
    {
    case RTC_FORMAT_FLOAT3X4_ROW_MAJOR:
      space = AffineSpace3fa(Vec3fa(xfm[ 0], xfm[ 4], xfm[ 8]),
                             Vec3fa(xfm[ 1], xfm[ 5], xfm[ 9]),
                             Vec3fa(xfm[ 2], xfm[ 6], xfm[10]),
                             Vec3fa(xfm[ 3], xfm[ 7], xfm[11]));
      break;

    case RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR:
      space = AffineSpace3fa(Vec3fa(xfm[ 0], xfm[ 1], xfm[ 2]),
                             Vec3fa(xfm[ 3], xfm[ 4], xfm[ 5]),
                             Vec3fa(xfm[ 6], xfm[ 7], xfm[ 8]),
                             Vec3fa(xfm[ 9], xfm[10], xfm[11]));
      break;

    case RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR:
      space = AffineSpace3fa(Vec3fa(xfm[ 0], xfm[ 1], xfm[ 2]),
                             Vec3fa(xfm[ 4], xfm[ 5], xfm[ 6]),
                             Vec3fa(xfm[ 8], xfm[ 9], xfm[10]),
                             Vec3fa(xfm[12], xfm[13], xfm[14]));
      break;

    default: 
      throw_RTCError(RTC_ERROR_INVALID_OPERATION, "invalid matrix format");
      break;
    }
    return space;
  }

  void storeTransform(const AffineSpace3fa& space, RTCFormat format, float* xfm)
  {
    switch (format)
    {
    case RTC_FORMAT_FLOAT3X4_ROW_MAJOR:
      xfm[ 0] = space.l.vx.x;  xfm[ 1] = space.l.vy.x;  xfm[ 2] = space.l.vz.x;  xfm[ 3] = space.p.x;
      xfm[ 4] = space.l.vx.y;  xfm[ 5] = space.l.vy.y;  xfm[ 6] = space.l.vz.y;  xfm[ 7] = space.p.y;
      xfm[ 8] = space.l.vx.z;  xfm[ 9] = space.l.vy.z;  xfm[10] = space.l.vz.z;  xfm[11] = space.p.z;
      break;

    case RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR:
      xfm[ 0] = space.l.vx.x;  xfm[ 1] = space.l.vx.y;  xfm[ 2] = space.l.vx.z;
      xfm[ 3] = space.l.vy.x;  xfm[ 4] = space.l.vy.y;  xfm[ 5] = space.l.vy.z;
      xfm[ 6] = space.l.vz.x;  xfm[ 7] = space.l.vz.y;  xfm[ 8] = space.l.vz.z;
      xfm[ 9] = space.p.x;     xfm[10] = space.p.y;     xfm[11] = space.p.z;
      break;

    case RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR:
      xfm[ 0] = space.l.vx.x;  xfm[ 1] = space.l.vx.y;  xfm[ 2] = space.l.vx.z;  xfm[ 3] = 0.f;
      xfm[ 4] = space.l.vy.x;  xfm[ 5] = space.l.vy.y;  xfm[ 6] = space.l.vy.z;  xfm[ 7] = 0.f;
      xfm[ 8] = space.l.vz.x;  xfm[ 9] = space.l.vz.y;  xfm[10] = space.l.vz.z;  xfm[11] = 0.f;
      xfm[12] = space.p.x;     xfm[13] = space.p.y;     xfm[14] = space.p.z;     xfm[15] = 1.f;
      break;

    default:
      throw_RTCError(RTC_ERROR_INVALID_OPERATION, "invalid matrix format");
      break;
    }
  }

  RTC_API void rtcSetGeometryTransform(RTCGeometry hgeometry, unsigned int timeStep, RTCFormat format, const void* xfm)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryTransform);
    RTC_VERIFY_HANDLE(hgeometry);
    RTC_VERIFY_HANDLE(xfm);
    const AffineSpace3fa transform = loadTransform(format, (const float*)xfm);
    geometry->setTransform(transform, timeStep);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcGetGeometryTransform(RTCGeometry hgeometry, float time, RTCFormat format, void* xfm)
  {
    Geometry* geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetGeometryTransform);
    const AffineSpace3fa transform = geometry->getTransform(time);
    storeTransform(transform, format, (float*)xfm);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcFilterIntersection(const struct RTCIntersectFunctionNArguments* const args_i, const struct RTCFilterFunctionNArguments* filter_args)
  {
    IntersectFunctionNArguments* args = (IntersectFunctionNArguments*) args_i;
    args->report(args,filter_args);
  }

  RTC_API void rtcFilterOcclusion(const struct RTCOccludedFunctionNArguments* const args_i, const struct RTCFilterFunctionNArguments* filter_args)
  {
    OccludedFunctionNArguments* args = (OccludedFunctionNArguments*) args_i;
    args->report(args,filter_args);
  }
  
  RTC_API RTCGeometry rtcNewGeometry (RTCDevice hdevice, RTCGeometryType type)
  {
    Device* device = (Device*) hdevice;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcNewGeometry);
    RTC_VERIFY_HANDLE(hdevice);
    
    switch (type)
    {
    case RTC_GEOMETRY_TYPE_TRIANGLE:
    {
#if defined(EMBREE_GEOMETRY_TRIANGLES)
      createTriangleMeshTy createTriangleMesh = nullptr;
      SELECT_SYMBOL_DEFAULT_AVX(device->enabled_cpu_features,createTriangleMesh);
      Geometry* geom = createTriangleMesh(device);
      return (RTCGeometry) geom->refInc();
#else
      throw_RTCError(RTC_ERROR_UNKNOWN,"RTC_GEOMETRY_TYPE_TRIANGLE is not supported");
#endif
    }
    
    case RTC_GEOMETRY_TYPE_QUAD:
    {
#if defined(EMBREE_GEOMETRY_QUADS)
      createQuadMeshTy createQuadMesh = nullptr;
      SELECT_SYMBOL_DEFAULT_AVX(device->enabled_cpu_features,createQuadMesh);
      Geometry* geom = createQuadMesh(device);
      return (RTCGeometry) geom->refInc();
#else
      throw_RTCError(RTC_ERROR_UNKNOWN,"RTC_GEOMETRY_TYPE_QUAD is not supported");
#endif
    }
    
    case RTC_GEOMETRY_TYPE_FLAT_LINEAR_CURVE:
    case RTC_GEOMETRY_TYPE_ROUND_BEZIER_CURVE:
    case RTC_GEOMETRY_TYPE_FLAT_BEZIER_CURVE:
    case RTC_GEOMETRY_TYPE_ROUND_BSPLINE_CURVE:
    case RTC_GEOMETRY_TYPE_FLAT_BSPLINE_CURVE:
    {
#if defined(EMBREE_GEOMETRY_CURVES)
      createLineSegmentsTy createLineSegments = nullptr;
      SELECT_SYMBOL_DEFAULT_AVX(device->enabled_cpu_features,createLineSegments);
      createCurvesBezierTy createCurvesBezier = nullptr;
      SELECT_SYMBOL_DEFAULT_AVX(device->enabled_cpu_features,createCurvesBezier);
      createCurvesBSplineTy createCurvesBSpline = nullptr;
      SELECT_SYMBOL_DEFAULT_AVX(device->enabled_cpu_features,createCurvesBSpline);
      
      Geometry* geom;
      switch (type) {
      case RTC_GEOMETRY_TYPE_FLAT_LINEAR_CURVE       : geom = createLineSegments (device); break;
      case RTC_GEOMETRY_TYPE_ROUND_BEZIER_CURVE : geom = createCurvesBezier (device,ROUND_CURVE); break;
      case RTC_GEOMETRY_TYPE_FLAT_BEZIER_CURVE  : geom = createCurvesBezier (device,FLAT_CURVE); break;
      case RTC_GEOMETRY_TYPE_ROUND_BSPLINE_CURVE: geom = createCurvesBSpline(device,ROUND_CURVE); break;
      case RTC_GEOMETRY_TYPE_FLAT_BSPLINE_CURVE : geom = createCurvesBSpline(device,FLAT_CURVE); break;
      default:                                    geom = nullptr; break;
      }
      return (RTCGeometry) geom->refInc();
#else
      throw_RTCError(RTC_ERROR_UNKNOWN,"RTC_GEOMETRY_TYPE_CURVE is not supported");
#endif
    }
    
    case RTC_GEOMETRY_TYPE_SUBDIVISION:
    {
#if defined(EMBREE_GEOMETRY_SUBDIVISION)
      createSubdivMeshTy createSubdivMesh = nullptr;
      SELECT_SYMBOL_DEFAULT_AVX(device->enabled_cpu_features,createSubdivMesh);
      Geometry* geom = createSubdivMesh(device);
      return (RTCGeometry) geom->refInc();
#else
      throw_RTCError(RTC_ERROR_UNKNOWN,"RTC_GEOMETRY_TYPE_SUBDIVISION is not supported");
#endif
    }
    
    case RTC_GEOMETRY_TYPE_USER:
    {
#if defined(EMBREE_GEOMETRY_USER)
      Geometry* geom = new UserGeometry(device,0,1);
      return (RTCGeometry) geom->refInc();
#else
      throw_RTCError(RTC_ERROR_UNKNOWN,"RTC_GEOMETRY_TYPE_USER is not supported");
#endif
    }

    case RTC_GEOMETRY_TYPE_INSTANCE:
    {
#if defined(EMBREE_GEOMETRY_USER)
      Geometry* geom = new Instance(device,nullptr,1);
      return (RTCGeometry) geom->refInc();
#else
      throw_RTCError(RTC_ERROR_UNKNOWN,"RTC_GEOMETRY_TYPE_INSTANCE is not supported");
#endif
    }
    
    default:
      throw_RTCError(RTC_ERROR_UNKNOWN,"invalid geometry type");
    }
    
    RTC_CATCH_END(device);
    return nullptr;
  }
  
  RTC_API void rtcSetGeometryUserPrimitiveCount(RTCGeometry hgeometry, unsigned int userPrimCount)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryUserPrimitiveCount);
    RTC_VERIFY_HANDLE(hgeometry);
    
    if (unlikely(geometry->type != Geometry::USER_GEOMETRY))
      throw_RTCError(RTC_ERROR_INVALID_OPERATION,"operation only allowed for user geometries"); 

    geometry->setNumPrimitives(userPrimCount);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryTimeStepCount(RTCGeometry hgeometry, unsigned int timeStepCount)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryTimeStepCount);
    RTC_VERIFY_HANDLE(hgeometry);

    if (timeStepCount > RTC_MAX_TIME_STEP_COUNT)
      throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"number of time steps is out of range");
    
    geometry->setNumTimeSteps(timeStepCount);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryVertexAttributeCount(RTCGeometry hgeometry, unsigned int N)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryVertexAttributeCount);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setVertexAttributeCount(N);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryTopologyCount(RTCGeometry hgeometry, unsigned int N)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryTopologyCount);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setTopologyCount(N);
    RTC_CATCH_END2(geometry);
  }
 
  /*! sets the build quality of the geometry */
  RTC_API void rtcSetGeometryBuildQuality (RTCGeometry hgeometry, RTCBuildQuality quality) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryBuildQuality);
    RTC_VERIFY_HANDLE(hgeometry);
    if (quality != RTC_BUILD_QUALITY_LOW &&
        quality != RTC_BUILD_QUALITY_MEDIUM &&
        quality != RTC_BUILD_QUALITY_HIGH &&
        quality != RTC_BUILD_QUALITY_REFIT)
      throw std::runtime_error("invalid build quality");
    geometry->setBuildQuality(quality);
    RTC_CATCH_END2(geometry);
  }
  
  RTC_API void rtcSetGeometryMask (RTCGeometry hgeometry, unsigned int mask) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryMask);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setMask(mask);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometrySubdivisionMode (RTCGeometry hgeometry, unsigned topologyID, RTCSubdivisionMode mode) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometrySubdivisionMode);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setSubdivisionMode(topologyID,mode);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryVertexAttributeTopology(RTCGeometry hgeometry, unsigned int vertexAttributeID, unsigned int topologyID)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryVertexAttributeTopology);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setVertexAttributeTopology(vertexAttributeID, topologyID);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryBuffer(RTCGeometry hgeometry, RTCBufferType type, unsigned int slot, RTCFormat format, RTCBuffer hbuffer, size_t byteOffset, size_t byteStride, size_t itemCount)
  {
    Ref<Geometry> geometry = (Geometry*)hgeometry;
    Ref<Buffer> buffer = (Buffer*)hbuffer;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryBuffer);
    RTC_VERIFY_HANDLE(hgeometry);
    RTC_VERIFY_HANDLE(hbuffer);
    if (geometry->device != buffer->device)
      throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"inputs are from different devices");
    geometry->setBuffer(type, slot, format, buffer, byteOffset, byteStride, itemCount);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetSharedGeometryBuffer(RTCGeometry hgeometry, RTCBufferType type, unsigned int slot, RTCFormat format, const void* ptr, size_t byteOffset, size_t byteStride, size_t itemCount)
  {
    Ref<Geometry> geometry = (Geometry*)hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetSharedGeometryBuffer);
    RTC_VERIFY_HANDLE(hgeometry);
    Ref<Buffer> buffer = new Buffer(geometry->device, itemCount*byteStride, (char*)ptr + byteOffset);
    geometry->setBuffer(type, slot, format, buffer, 0, byteStride, itemCount);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void* rtcSetNewGeometryBuffer(RTCGeometry hgeometry, RTCBufferType type, unsigned int slot, RTCFormat format, size_t byteStride, size_t itemCount)
  {
    Ref<Geometry> geometry = (Geometry*)hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetNewGeometryBuffer);
    RTC_VERIFY_HANDLE(hgeometry);
    Ref<Buffer> buffer = new Buffer(geometry->device, itemCount*byteStride);
    geometry->setBuffer(type, slot, format, buffer, 0, byteStride, itemCount);
    return buffer->data();
    RTC_CATCH_END2(geometry);
    return nullptr;
  }

  RTC_API void* rtcGetGeometryBufferData(RTCGeometry hgeometry, RTCBufferType type, unsigned int slot)
  {
    Ref<Geometry> geometry = (Geometry*)hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetGeometryBufferData);
    RTC_VERIFY_HANDLE(hgeometry);
    return geometry->getBuffer(type, slot);
    RTC_CATCH_END2(geometry);
    return nullptr;
  }
  
  RTC_API void rtcEnableGeometry (RTCGeometry hgeometry) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcEnableGeometry);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->enable();
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcUpdateGeometryBuffer (RTCGeometry hgeometry, RTCBufferType type, unsigned int slot) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcUpdateGeometryBuffer);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->updateBuffer(type, slot);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcDisableGeometry (RTCGeometry hgeometry) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcDisableGeometry);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->disable();
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryTessellationRate (RTCGeometry hgeometry, float tessellationRate)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryTessellationRate);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setTessellationRate(tessellationRate);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryUserData (RTCGeometry hgeometry, void* ptr) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryUserData);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setUserData(ptr);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void* rtcGetGeometryUserData (RTCGeometry hgeometry)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetGeometryUserData);
    RTC_VERIFY_HANDLE(hgeometry);
    return geometry->getUserData();
    RTC_CATCH_END2(geometry);
    return nullptr;
  }

  RTC_API void rtcSetGeometryBoundsFunction (RTCGeometry hgeometry, RTCBoundsFunction bounds, void* userPtr)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryBoundsFunction);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setBoundsFunction(bounds,userPtr);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryDisplacementFunction (RTCGeometry hgeometry, RTCDisplacementFunction func)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryDisplacementFunction);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setDisplacementFunction(func);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryIntersectFunction (RTCGeometry hgeometry, RTCIntersectFunctionN intersect) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryIntersectFunction);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setIntersectFunctionN(intersect);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryOccludedFunction (RTCGeometry hgeometry, RTCOccludedFunctionN occluded) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetOccludedFunctionN);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setOccludedFunctionN(occluded);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryIntersectFilterFunction (RTCGeometry hgeometry, RTCFilterFunctionN intersect) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryIntersectFilterFunction);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setIntersectionFilterFunctionN(intersect);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcSetGeometryOccludedFilterFunction (RTCGeometry hgeometry, RTCFilterFunctionN intersect) 
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcSetGeometryOccludedFilterFunction);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->setOcclusionFilterFunctionN(intersect);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcInterpolate(const RTCInterpolateArguments* const args)
  {
    Geometry* geometry = (Geometry*) args->geometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcInterpolate);
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(args->geometry);
#endif
    geometry->interpolate(args);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcInterpolateN(const RTCInterpolateNArguments* const args)
  {
    Geometry* geometry = (Geometry*) args->geometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcInterpolateN);
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(args->geometry);
#endif
    geometry->interpolateN(args);
    RTC_CATCH_END2(geometry);
  }

  RTC_API void rtcCommitGeometry (RTCGeometry hgeometry)
  {
    Geometry* geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcCommitGeometry);
    RTC_VERIFY_HANDLE(hgeometry);
    return geometry->commit();
    RTC_CATCH_END2(geometry);
  }

  RTC_API unsigned int rtcAttachGeometry (RTCScene hscene, RTCGeometry hgeometry)
  {
    Scene* scene = (Scene*) hscene;
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcAttachGeometry);
    RTC_VERIFY_HANDLE(hscene);
    RTC_VERIFY_HANDLE(hgeometry);
    if (scene->device != geometry->device)
      throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"inputs are from different devices");
    return scene->bind(RTC_INVALID_GEOMETRY_ID,geometry);
    RTC_CATCH_END2(scene);
    return -1;
  }

  RTC_API void rtcAttachGeometryByID (RTCScene hscene, RTCGeometry hgeometry, unsigned int geomID)
  {
    Scene* scene = (Scene*) hscene;
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcAttachGeometryByID);
    RTC_VERIFY_HANDLE(hscene);
    RTC_VERIFY_HANDLE(hgeometry);
    RTC_VERIFY_GEOMID(geomID);
    if (scene->device != geometry->device)
      throw_RTCError(RTC_ERROR_INVALID_ARGUMENT,"inputs are from different devices");
    unsigned int gID = scene->bind(geomID,geometry);
    assert(gID == geomID);
    RTC_CATCH_END2(scene);
  }
  
  RTC_API void rtcDetachGeometry (RTCScene hscene, unsigned int geomID)
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcDetachGeometry);
    RTC_VERIFY_HANDLE(hscene);
    RTC_VERIFY_GEOMID(geomID);
    scene->detachGeometry(geomID);
    RTC_CATCH_END2(scene);
  }

  RTC_API void rtcRetainGeometry (RTCGeometry hgeometry)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcRetainGeometry);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->refInc();
    RTC_CATCH_END2(geometry);
  }
  
  RTC_API void rtcReleaseGeometry (RTCGeometry hgeometry)
  {
    Ref<Geometry> geometry = (Geometry*) hgeometry;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcReleaseGeometry);
    RTC_VERIFY_HANDLE(hgeometry);
    geometry->refDec();
    RTC_CATCH_END2(geometry);
  }

  RTC_API RTCGeometry rtcGetGeometry (RTCScene hscene, unsigned int geomID)
  {
    Scene* scene = (Scene*) hscene;
    RTC_CATCH_BEGIN;
    RTC_TRACE(rtcGetGeometry);
#if defined(DEBUG)
    RTC_VERIFY_HANDLE(hscene);
    RTC_VERIFY_GEOMID(geomID);
#endif
    return (RTCGeometry) scene->get(geomID);
    RTC_CATCH_END2(scene);
    return nullptr;
  }
}
