#if !defined RAY_LANE
# define RAY_LANE

#if USE_SIMD
# define LANE_WIDTH 4
#else
# define LANE_WIDTH 1
#endif

struct vec3
{
	float x, y, z;
};

///
/// 4-wide SIMD
/// 
#if (LANE_WIDTH==4)
#include <intrin.h>

#include "ray_lane_4.h"

///
/// 1-wide float
///
#elif (LANE_WIDTH==1)
typedef f32 lane_f32;
typedef u32 lane_u32;
typedef vec3 lane_v3;

lane_v3 operator+(lane_v3 a, lane_v3 b)
{
	lane_v3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;

	return result;
}

lane_v3 operator&(lane_u32 a, lane_v3 b)
{
	lane_v3 result;
	// TODO: maybe change it to separate function
	a = (a ? 0xFFFFFFFF : 0);
	u32 x = a & *(u32*)&b.x;
	u32 y = a & *(u32*)&b.y;
	u32 z = a & *(u32*)&b.z;
	result.x = *(f32*)&x;
	result.y = *(f32*)&y;
	result.z = *(f32*)&z;

	return result;
}

lane_u32 LaneU32FromU32(u32 a)
{
	lane_u32 result = a;

	return result;
}

lane_u32 LaneU32FromU32(u32 a0, u32 a1, u32 a2, u32 a3)
{
	// TODO: Maybe should do something about this?
	lane_u32 result = a0;

	return result;
}

lane_f32 LaneF32FromU32(lane_u32 a)
{
	lane_f32 result = (lane_f32)a;

	return result;
}

lane_f32 LaneF32FromF32(f32 a)
{
	lane_f32 result = a;

	return result;
}

void ConditionalAssign(lane_u32* dest, lane_u32 mask, lane_u32 source)
{
	mask = mask ? 0xffffffff : 0;
	*dest = (~mask & *dest) | (mask & source);
}

void ConditionalAssign(lane_f32* dest, lane_u32 mask, lane_f32 source)
{
	ConditionalAssign((lane_u32*)dest, mask, *(lane_u32*)&source);
}

lane_f32 Max(lane_f32 a, lane_f32 b)
{
	lane_f32 result = a > b ? a : b;
	return result;
}

bool MaskIsZero(lane_u32 mask)
{
	bool result = (mask == 0);
	return result;
}

u32 HorizontalAdd(lane_u32 a)
{
	u32 result = a;
	return result;
}

f32 HorizontalAdd(lane_f32 a)
{
	f32 result = a;
	return result;
}

lane_f32 GatherF32_(void* basePtr, u32 stride, lane_u32 index)
{
	lane_f32 result = (*(f32*)((u8*)basePtr + index * stride));

	return result;
}

#else
#error LANE_WIDTH should be 1 or 4
#endif

#if LANE_WIDTH != 1

#endif

vec3 Extract0(lane_v3 a)
{
	vec3 result;
	result.x = *(f32*)&a.x;
	result.y = *(f32*)&a.y;
	result.z = *(f32*)&a.z;

	return result;
}

void ConditionalAssign(lane_v3* dest, lane_u32 mask, lane_v3 source)
{
	ConditionalAssign(&dest->x, mask, source.x);
	ConditionalAssign(&dest->y, mask, source.y);
	ConditionalAssign(&dest->z, mask, source.z);
}


vec3 HorizontalAdd(lane_v3 a)
{
	vec3 result =
	{
		HorizontalAdd(a.x),
		HorizontalAdd(a.y),
		HorizontalAdd(a.z)
	};
	return result;
}

lane_v3 LaneV3FromV3(vec3 v)
{
	lane_v3 result;
	result.x = LaneF32FromF32(v.x);
	result.y = LaneF32FromF32(v.y);
	result.z = LaneF32FromF32(v.z);

	return result;
}

lane_v3 GatherV3_(void* basePtr, u32 stride, lane_u32 indices)
{
	lane_v3 result;
	result.x = GatherF32_((f32*)basePtr + 0, stride, indices);
	result.y = GatherF32_((f32*)basePtr + 1, stride, indices);
	result.z = GatherF32_((f32*)basePtr + 2, stride, indices);

	return result;
}

#define GATHER_F32(basePtr, index, member) GatherF32_(&(basePtr)->member, sizeof(*(basePtr)), index)
#define GATHER_V3(basePtr, index, member) GatherV3_(&(basePtr)->member, sizeof(*(basePtr)), index)

#endif