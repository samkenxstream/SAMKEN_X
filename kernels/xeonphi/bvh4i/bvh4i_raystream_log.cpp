// ======================================================================== //
// Copyright 2009-2014 Intel Corporation                                    //
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

#include "bvh4i_raystream_log.h"
#include "sys/filename.h"

#define RAYSTREAM_FILENAME "/home/micuser/ray16log.bin"
#define BVH_FILENAME       "/home/micuser/bvhlog.bin"
#define ACCEL_FILENAME     "/home/micuser/accellog.bin"


namespace embree
{
  using namespace std;

  enum { 
    RAY_INTERSECT = 0,
    RAY_OCCLUDED  = 1
  };
	 

  RayStreamLogger::RayStreamLogger() : initialized(false)
  {
    int error = pthread_mutex_init(&mutex,NULL);
  }


  RayStreamLogger::~RayStreamLogger()
    {
      PING;

      if (initialized)
	{
	  rayData.close();
	}
    }

  void RayStreamLogger::openRayDataStream()
  {
    FileName filename(RAYSTREAM_FILENAME);
    rayData.open(filename.c_str(),ios::out | ios::binary);
    rayData.seekp(0, ios::beg);
 
    if (!rayData)
      {
	FATAL("could not dump ray data to file");
      }    
  }

  void RayStreamLogger::storeBVH(BVH4i* bvh)
  {
    std::ofstream bvhData;
    FileName bvh_filename(BVH_FILENAME);
    bvhData.open(bvh_filename.c_str(),ios::out | ios::binary);
    if (!bvhData) FATAL("could not dump bvh data to file");
    bvhData.seekp(0, ios::beg);
    bvhData.write((char*)bvh->nodePtr(),bvh->size_node);
    bvhData.close();

    std::ofstream accelData;

    FileName accel_filename(ACCEL_FILENAME);
    accelData.open(accel_filename.c_str(),ios::out | ios::binary);
    if (!accelData) FATAL("could not dump accel data to file");
    accelData.seekp(0, ios::beg);
    accelData.write((char*)bvh->triPtr(),bvh->size_accel);
    accelData.close();    
  }

  void RayStreamLogger::logRay16Intersect(mic_i* valid_i, BVH4i* bvh, Ray16& start, Ray16& end)
  {
    pthread_mutex_lock(&mutex);

    if (!initialized)
      {
	storeBVH(bvh);
	openRayDataStream();
	initialized = true;
      }

    LogRay16 logRay16;

    logRay16.type    = RAY_INTERSECT;
    logRay16.m_valid = *valid_i != mic_i(0);
    logRay16.start   = start;
    logRay16.end     = end;
    rayData.write((char*)&logRay16 ,sizeof(logRay16));
    rayData << flush;
    pthread_mutex_unlock(&mutex);
  }

  void RayStreamLogger::logRay16Occluded(mic_i* valid_i, BVH4i* bvh, Ray16& start, Ray16& end)
  {
    pthread_mutex_lock(&mutex);
    if (!initialized)
      {
	storeBVH(bvh);
	openRayDataStream();
	initialized = true;
      }

    LogRay16 logRay16;

    logRay16.type    = RAY_OCCLUDED;
    logRay16.m_valid = *valid_i != mic_i(0);
    logRay16.start   = start;
    logRay16.end     = end;
    rayData.write((char*)&logRay16 ,sizeof(logRay16));
    rayData << flush;
    pthread_mutex_unlock(&mutex);
  }

  RayStreamLogger RayStreamLogger::rayStreamLogger;

};
