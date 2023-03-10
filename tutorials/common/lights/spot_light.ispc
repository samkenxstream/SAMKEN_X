// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "light.isph"
#include "../math/sampling.isph"
#include "../math/linearspace.isph"

struct SpotLight
{
  Light super;            //!< inherited light fields

  Vec3f position;         //!< Position of the SpotLight
  LinearSpace3f frame;         //!< coordinate frame, with vz == direction that the SpotLight is emitting
  Vec3f power;            //!< RGB color and intensity of the SpotLight
  float cosAngleMax;      //!< Angular limit of the spot in an easier to use form: cosine of the half angle in radians
  float cosAngleScale;    //!< 1/(cos(border of the penumbra area) - cosAngleMax); positive
  float radius;           //!< defines the size of the (extended) SpotLight
  float diskPdf;          //!< pdf of disk with radius
};


// Implementation
//////////////////////////////////////////////////////////////////////////////

SYCL_EXTERNAL Light_SampleRes SpotLight_sample(const uniform Light* uniform super,
                                                              const DifferentialGeometry& dg,
                                                              const Vec2f& s)
{
  const SpotLight* uniform self = (SpotLight* uniform)super;
  Light_SampleRes res;

  // extant light vector from the hit point
  res.dir = self->position - dg.P;

  if (self->radius > 0.0f)
    res.dir = self->frame * uniformSampleDisk(self->radius, s) + res.dir;

  const float dist2 = dot(res.dir, res.dir);
  const float invdist = rsqrt(dist2);

  // normalized light vector
  res.dir = res.dir * invdist;
  res.dist = dist2 * invdist;

  // cosine of the negated light direction and light vector.
  const float cosAngle = -dot(self->frame.vz, res.dir);
  const float angularAttenuation = clamp((cosAngle - self->cosAngleMax) * self->cosAngleScale);

  if (self->radius > 0.0f)
    res.pdf = self->diskPdf * dist2 * abs(cosAngle);
  else
    res.pdf = inf; // we always take this res

  // convert from power to radiance by attenuating by distance^2; attenuate by angle
  res.weight = self->power * (sqr(invdist) * angularAttenuation);

  return res;
}

SYCL_EXTERNAL Light_EvalRes SpotLight_eval(const uniform Light* uniform super,
                                                          const DifferentialGeometry& dg,
                                                          const Vec3f& dir)
{
  const SpotLight* uniform self = (SpotLight* uniform)super;
  Light_EvalRes res;
  res.value = make_Vec3f(0.0f);
  res.dist = inf;
  res.pdf = 0.0f;

  if (self->radius > 0.0f) {
    // intersect disk
    const float cosAngle = -dot(dir, self->frame.vz);
    if (cosAngle > self->cosAngleMax) { // inside illuminated cone?
      const Vec3f vp = dg.P - self->position;
      const float dp = dot(vp, self->frame.vz);
      if (dp > 0.0f) { // in front of light?
        const float t = dp*rcp(cosAngle);
        const Vec3f vd = vp + t * dir;
        if (dot(vd, vd) < sqr(self->radius)) { // inside disk?
          const float angularAttenuation = min((cosAngle - self->cosAngleMax) * self->cosAngleScale, 1.f);
          const float pdf = self->diskPdf * cosAngle;
          res.value = self->power * (angularAttenuation * pdf); // *sqr(t)/sqr(t) cancels
          res.dist = t;
          res.pdf = pdf * sqr(t);
        }
      }
    }
  }

  return res;
}


// Exports (called from C++)
//////////////////////////////////////////////////////////////////////////////

//! Set the parameters of an ispc-side SpotLight object
export void SpotLight_set(void* uniform super,
                          const uniform Vec3f& position,
                          const uniform Vec3f& direction,
                          const uniform Vec3f& power,
                          uniform float cosAngleMax,
                          uniform float cosAngleScale,
                          uniform float radius)
{
  uniform SpotLight* uniform self = (uniform SpotLight* uniform)super;
  self->position      = position;
  self->frame         = frame(direction);
  self->power         = power;
  self->cosAngleMax   = cosAngleMax;
  self->cosAngleScale = cosAngleScale;
  self->radius        = radius;
  self->diskPdf       = uniformSampleDiskPDF(radius);
}

//! Create an ispc-side SpotLight object
export void* uniform SpotLight_create()
{
  uniform SpotLight* uniform self = uniform new uniform SpotLight;

  Light_Constructor(&self->super);
  //self->super.sample = GET_FUNCTION_POINTER(SpotLight_sample);
  //self->super.eval = GET_FUNCTION_POINTER(SpotLight_eval);
  self->super.type = LIGHT_SPOT;

  SpotLight_set(self,
                make_Vec3f(0.f),
                make_Vec3f(0.f, 0.f, 1.f),
                make_Vec3f(1.f),
                0.f,
                100.f,
                0.f);

  return self;
}
