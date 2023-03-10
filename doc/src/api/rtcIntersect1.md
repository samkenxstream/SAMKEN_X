% rtcIntersect1(3) | Embree Ray Tracing Kernels 4

#### NAME

    rtcIntersect1 - finds the closest hit for a single ray

#### SYNOPSIS

    #include <embree4/rtcore.h>

    void rtcIntersect1(
      RTCScene scene,
      struct RTCRayHit* rayhit
      struct RTCIntersectArguments* args = NULL
    );

#### DESCRIPTION

The `rtcIntersect1` function finds the closest hit of a single ray
(`rayhit` argument) with the scene (`scene` argument). The provided
ray/hit structure contains the ray to intersect and some hit output
fields that are filled when a hit is found. The passed optional
arguments struct (`args` argument) can get used for advanced use
cases, see section [rtcInitIntersectArguments] for more details.

To trace a ray, the user has to initialize the ray origin (`org` ray
member), ray direction (`dir` ray member), ray segment (`tnear`,
`tfar` ray members), ray mask (`mask` ray member), and set the ray
flags to `0` (`flags` ray member). The ray time (`time` ray member)
must be initialized to a value in the range $[0, 1]. The ray segment
has to be in the range $[0, \infty]$, thus ranges that start behind
the ray origin are not valid, but ranges can reach to infinity. See
Section [RTCRay] for the ray layout description.

The geometry ID (`geomID` hit member) of the hit data must be initialized to
`RTC_INVALID_GEOMETRY_ID` (-1).

When no intersection is found, the ray/hit data is not updated. When an
intersection is found, the hit distance is written into the `tfar`
member of the ray and all hit data is set, such as unnormalized
geometry normal in object space (`Ng` hit member), local hit
coordinates (`u`, `v` hit member), instance ID stack (`instID` hit member),
geometry ID (`geomID` hit member), and primitive ID (`primID` hit
member). See Section [RTCHit] for the hit layout description.

If the instance ID stack has a prefix of values not equal to
`RTC_INVALID_GEOMETRY_ID`, the instance ID on each level corresponds to the geometry
ID of the hit instance of the higher-level scene, the geometry ID
corresponds to the hit geometry inside the hit instanced scene, and the
primitive ID corresponds to the n-th primitive of that geometry.

If level 0 of the instance ID stack is equal to
`RTC_INVALID_GEOMETRY_ID`, the geometry ID corresponds to the hit
geometry inside the top-level scene, and the primitive ID corresponds to the
n-th primitive of that geometry.

The implementation makes no guarantees that primitives whose hit
distance is exactly at (or very close to) `tnear` or `tfar` are hit or
missed. If you want to exclude intersections at `tnear` just pass a
slightly enlarged `tnear`, and if you want to include intersections at
`tfar` pass a slightly enlarged `tfar`.

``` {include=src/api/inc/raypointer.md}
```

The ray/hit structure must be aligned to 16 bytes.

#### EXIT STATUS

For performance reasons this function does not do any error checks,
thus will not set any error flags on failure.

#### SEE ALSO

[rtcOccluded1], [rtcIntersect4/8/16], [RTCRayHit], [rtcInitIntersectArguments]
