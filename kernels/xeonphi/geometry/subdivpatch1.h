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

#pragma once

#include "primitive.h"
#include "common/scene_subdiv_mesh.h"
#include "common/scene_subdivision.h"
#include "bicubic_bezier_patch.h"

// right now everything is shared between xeon and xeon phi, so moved all stuff to common/scene_subdivision.h

namespace embree
{

  struct SubdivPatch1
  {
  public:
    enum {
      REGULAR_PATCH = 1,
      HAS_BORDERS   = 2,
      HAS_CREASES   = 4,
      GREGORY_PATCH = 8
    };

    /*! Default constructor. */
    __forceinline SubdivPatch1 (const SubdivMesh::HalfEdge * first_half_edge,
				const Vec3fa *vertices,
				unsigned int geomID,
				unsigned int primID,
				unsigned int subdivision_level = 0) 
      : first_half_edge(first_half_edge),
      vertices(vertices),
      geomID(geomID),
      primID(primID),
      subdivision_level(subdivision_level)
    {
      u_val = Vec2f(0.0f,1.0f);
      v_val = Vec2f(0.0f,1.0f);
      f_m[0][0] = 0.0f;
      f_m[0][1] = 0.0f;
      f_m[1][1] = 0.0f;
      f_m[1][0] = 0.0f;

      flags = 0;
      if (first_half_edge->isFaceRegular() && 0) 
	{
	  flags |= REGULAR_PATCH;
	  init( patch );
	  /* DBG_PRINT( patch ); */




	  /* DBG_PRINT(gpatch); */
	  /* exit(0); */	  
	}
#if 1
      else if (!first_half_edge->faceHasEdges() || 1)
	{
	  flags |= GREGORY_PATCH;

#if 1
	  RegularCatmullClarkPatch rpatch;

	  init( rpatch );

	  BicubicBezierPatch bpatch;

	  bpatch.init( rpatch.v );


#endif

	  GregoryPatch gpatch; 
	  gpatch.init( first_half_edge, vertices ); 
	  
	  //DBG_PRINT(gpatch);

	  gpatch.exportConrolPoints( patch.v, f_m );
#if 0

	  DBG_PRINT( rpatch.eval(0.0f,0.0f) ); 
	  DBG_PRINT( gpatch.eval(0.0f,0.0f) );

	  DBG_PRINT( rpatch.eval(0.0f,1.0f) ); 
	  DBG_PRINT( gpatch.eval(0.0f,1.0f) );

	  DBG_PRINT( rpatch.eval(1.0f,1.0f) ); 
	  DBG_PRINT( gpatch.eval(1.0f,1.0f) );

	  DBG_PRINT( rpatch.eval(1.0f,0.0f) ); 
	  DBG_PRINT( gpatch.eval(1.0f,0.0f) );

	  DBG_PRINT( rpatch.eval(0.5f,0.5f) ); 
	  DBG_PRINT( gpatch.eval(0.5f,0.5f) );

	  DBG_PRINT( rpatch.eval(0.25f,0.25f) ); 
	  DBG_PRINT( gpatch.eval(0.25f,0.25f) );
	  DBG_PRINT( bpatch.eval(0.25f,0.25f) );

	  DBG_PRINT(bpatch);

	  exit(0);

	  DBG_PRINT( rpatch.eval(0.75f,0.75f) ); 
	  DBG_PRINT( gpatch.eval(0.75f,0.75f) );

	  DBG_PRINT( rpatch.eval(0.25f,0.75f) ); 
	  DBG_PRINT( gpatch.eval(0.25f,0.75f) );

	  DBG_PRINT( rpatch.eval(0.75f,0.25f) ); 
	  DBG_PRINT( gpatch.eval(0.75f,0.25f) );

#endif

	}
#endif
      else
	{
	}
    }

    __forceinline bool isRegular() const
    {
      return (flags & REGULAR_PATCH) == REGULAR_PATCH;
    }

    __forceinline bool isGregoryPatch() const
    {
      return (flags & GREGORY_PATCH) == GREGORY_PATCH;
    }

    __forceinline const Vec3fa &getQuadVertex(const unsigned int i=0) const { 
      const SubdivMesh::HalfEdge *const h = first_half_edge + i;
      return vertices[h->vtx_index];
    }

    __forceinline void init( IrregularCatmullClarkPatch& patch) const
    {
      for (size_t i=0;i<4;i++)
	patch.ring[i].init(first_half_edge + i,vertices);
    }

    __forceinline void init( FinalQuad& quad ) const
    {
      quad.vtx[0] = getQuadVertex(0);
      quad.vtx[1] = getQuadVertex(1);
      quad.vtx[2] = getQuadVertex(2);
      quad.vtx[3] = getQuadVertex(3);
    };

    __forceinline void init( RegularCatmullClarkPatch& cc_patch) const
    {
      cc_patch.init(first_half_edge, vertices);
    }
   
    const SubdivMesh::HalfEdge * first_half_edge; //!< pointer to first half edge of corresponding quad in the subdivision mesh
    const Vec3fa *vertices;                       //!< pointer to the vertex positions in the subdivison mesh
    unsigned int flags;
    unsigned int subdivision_level;
    unsigned int geomID;                          //!< geometry ID of the subdivision mesh this patch belongs to
    unsigned int primID;                          //!< primitive ID of this subdivision patch

    Vec2f u_val;
    Vec2f v_val;
    RegularCatmullClarkPatch patch;
    Vec3fa f_m[2][2];    
  };

  __forceinline std::ostream &operator<<(std::ostream &o, const SubdivPatch1 &p)
    {
      o << "first_half_edge " << p.first_half_edge << " vertices " << p.vertices << " flags " << p.flags << " geomID " << p.geomID << " primID " << p.primID;

      return o;
    } 

};

