// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "light.isph"
#include "../math/sampling.isph"
#include "../math/linearspace.isph"

struct DirectionalLight
{
  Light super;      //!< inherited light fields

  LinearSpace3f frame;   //!< coordinate frame, with vz == direction *towards* the light source
  Vec3f radiance;   //!< RGB color and intensity of light
  float cosAngle;   //!< Angular limit of the cone light in an easier to use form: cosine of the half angle in radians
  float pdf;        //!< Probability to sample a direction to the light
};

// for very small cones treat as singular light, because float precision is not good enough
#define COS_ANGLE_MAX 0.99999988f


// Implementation
//////////////////////////////////////////////////////////////////////////////

SYCL_EXTERNAL Light_SampleRes DirectionalLight_sample(const uniform Light* uniform super,
                                                                     const DifferentialGeometry& dg,
                                                                     const Vec2f& s)
{
  const DirectionalLight* uniform self = (DirectionalLight* uniform)super;
  Light_SampleRes res;

  res.dir = self->frame.vz;
  res.dist = inf;
  res.pdf = self->pdf;

  if (self->cosAngle < COS_ANGLE_MAX)
    res.dir = self->frame * uniformSampleCone(self->cosAngle, s);

  res.weight = self->radiance; // *pdf/pdf cancel

  return res;
}

SYCL_EXTERNAL Light_EvalRes DirectionalLight_eval(const uniform Light* uniform super,
                                                                 const DifferentialGeometry&,
                                                                 const Vec3f& dir)
{
  uniform DirectionalLight* uniform self = (uniform DirectionalLight* uniform)super;
  Light_EvalRes res;
  res.dist = inf;

  if (self->cosAngle < COS_ANGLE_MAX && dot(self->frame.vz, dir) > self->cosAngle) {
    res.value = self->radiance * self->pdf;
    res.pdf = self->pdf;
  } else {
    res.value = make_Vec3f(0.f);
    res.pdf = 0.f;
  }

  return res;
}


// Exports (called from C++)
//////////////////////////////////////////////////////////////////////////////

//! Set the parameters of an ispc-side DirectionalLight object
export void DirectionalLight_set(void* uniform super,
                                 const uniform Vec3f& direction,
                                 const uniform Vec3f& radiance,
                                 uniform float cosAngle)
{
  uniform DirectionalLight* uniform self = (uniform DirectionalLight* uniform)super;
  self->frame = frame(direction);
  self->radiance = radiance;
  self->cosAngle = cosAngle;
  self->pdf = cosAngle < COS_ANGLE_MAX ? uniformSampleConePDF(cosAngle) : inf;
}

//! Create an ispc-side DirectionalLight object
export void* uniform DirectionalLight_create()
{
  uniform DirectionalLight* uniform self = uniform new uniform DirectionalLight;
  Light_Constructor(&self->super);
  //self->super.sample = GET_FUNCTION_POINTER(DirectionalLight_sample);
  //self->super.eval = GET_FUNCTION_POINTER(DirectionalLight_eval);
  self->super.type = LIGHT_DIRECTIONAL;

  DirectionalLight_set(self, make_Vec3f(0.f, 0.f, 1.f), make_Vec3f(1.f), 1.f);
  return self;
}
