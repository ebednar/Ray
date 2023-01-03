#if !defined RAY_LANE_4
# define RAY_LANE_4

struct lane_f32
{
	__m128 v;
	lane_f32& operator=(f32 a);
};
struct lane_u32
{
	__m128i v;
	lane_u32& operator=(u32 a);
};

struct lane_v3
{
	lane_f32 x;
	lane_f32 y;
	lane_f32 z;
};

lane_u32 operator^(lane_u32 a, lane_u32 b)
{
	lane_u32 result;
	result.v = _mm_xor_si128(a.v, b.v);

	return result;
}

lane_u32 operator^=(lane_u32& a, lane_u32 b)
{
	a = a ^ b;

	return a;
}

lane_u32 operator&(lane_u32 a, lane_u32 b)
{
	lane_u32 result;
	result.v = _mm_and_si128(a.v, b.v);

	return result;
}

lane_u32 operator&=(lane_u32& a, lane_u32 b)
{
	a = a & b;

	return a;
}

lane_f32 operator&(lane_u32 a, lane_f32 b)
{
	lane_f32 result;
	result.v = _mm_and_ps(_mm_castsi128_ps(a.v), b.v);

	return result;
}

lane_v3 operator&(lane_u32 a, lane_v3 b)
{
	lane_v3 result;
	result.x = a & b.x;
	result.y = a & b.y;
	result.z = a & b.z;

	return result;
}

lane_u32 AndNot(lane_u32 a, lane_u32 b)
{
	lane_u32 result;
	result.v = _mm_andnot_si128(a.v, b.v);

	return result;
}

lane_u32 LaneU32FromU32(u32 a)
{
	lane_u32 result;
	result.v = _mm_set1_epi32(a);

	return result;
}

lane_u32 LaneU32FromU32(u32 a0, u32 a1, u32 a2, u32 a3)
{
	lane_u32 result;
	result.v = _mm_setr_epi32(a0, a1, a2, a3);

	return result;
}

lane_f32 LaneF32FromU32(lane_u32 a)
{
	lane_f32 result;
	result.v = _mm_cvtepi32_ps(a.v);

	return result;
}

lane_f32 LaneF32FromU32(u32 a)
{
	lane_f32 result;
	result.v = _mm_set1_ps((f32)a);

	return result;
}

lane_f32 LaneF32FromF32(f32 a)
{
	lane_f32 result;
	result.v = _mm_set1_ps(a);

	return result;
}

lane_u32 operator|(lane_u32 a, lane_u32 b)
{
	lane_u32 result;
	result.v = _mm_or_si128(a.v, b.v);

	return result;
}

lane_u32 operator<<(lane_u32 a, u32 shift)
{
	lane_u32 result;
	result.v = _mm_slli_epi32(a.v, shift);

	return result;
}

lane_u32 operator>>(lane_u32 a, u32 shift)
{
	lane_u32 result;
	result.v = _mm_srli_epi32(a.v, shift);

	return result;
}

lane_u32 operator<(lane_f32 a, lane_f32 b)
{
	lane_u32 result;
	result.v = _mm_castps_si128(_mm_cmplt_ps(a.v, b.v));

	return result;
}

lane_u32 operator<(lane_f32 a, f32 b)
{
	lane_u32 result = a < LaneF32FromF32(b);

	return result;
}

lane_u32 operator<(f32 a, lane_f32 b)
{
	lane_u32 result = LaneF32FromF32(a) < b;

	return result;
}

lane_u32 operator<=(lane_f32 a, lane_f32 b)
{
	lane_u32 result;
	result.v = _mm_castps_si128(_mm_cmple_ps(a.v, b.v));

	return result;
}

lane_u32 operator>(lane_f32 a, lane_f32 b)
{
	lane_u32 result;
	result.v = _mm_castps_si128(_mm_cmpgt_ps(a.v, b.v));

	return result;
}

lane_u32 operator>(lane_f32 a, f32 b)
{
	lane_u32 result = a > LaneF32FromF32(b);

	return result;
}

lane_u32 operator>(f32 a, lane_f32 b)
{
	lane_u32 result = LaneF32FromF32(a) > b;

	return result;
}

lane_u32 operator>=(lane_f32 a, lane_f32 b)
{
	lane_u32 result;
	result.v = _mm_castps_si128(_mm_cmpge_ps(a.v, b.v));

	return result;
}

lane_u32 operator==(lane_f32 a, lane_f32 b)
{
	lane_u32 result;
	result.v = _mm_castps_si128(_mm_cmpeq_ps(a.v, b.v));

	return result;
}

lane_u32 operator!=(lane_f32 a, lane_f32 b)
{
	lane_u32 result;
	result.v = _mm_castps_si128(_mm_cmpneq_ps(a.v, b.v));

	return result;
}

lane_u32 operator!=(lane_u32 a, lane_u32 b)
{
	lane_u32 result;
	result.v = _mm_xor_si128(_mm_cmpeq_epi32(a.v, b.v), _mm_set1_epi32(0xFFFFFFFF));

	return result;
}

lane_u32& lane_u32::operator=(u32 b)
{
	*this = LaneU32FromU32(b);

	return *this;
}

lane_f32& lane_f32::operator=(f32 b)
{
	*this = LaneF32FromF32(b);

	return *this;
}

lane_f32 operator+(lane_f32 a, lane_f32 b)
{
	lane_f32 result;
	result.v = _mm_add_ps(a.v, b.v);

	return result;
}

lane_f32 operator+(lane_f32 a, f32 b)
{
	lane_f32 result = a + LaneF32FromF32(b);

	return result;
}

lane_f32 operator+(f32 a, lane_f32 b)
{
	lane_f32 result = LaneF32FromF32(a) + b;

	return result;
}

lane_f32 operator+=(lane_f32& a, lane_f32 b)
{
	a = a + b;

	return a;
}

lane_u32 operator+(lane_u32 a, lane_u32 b)
{
	lane_u32 result;
	result.v = _mm_add_epi32(a.v, b.v);

	return result;
}

lane_u32 operator+=(lane_u32& a, lane_u32 b)
{
	a = a + b;

	return a;
}

lane_f32 operator-(lane_f32 a, lane_f32 div)
{
	lane_f32 result;
	result.v = _mm_sub_ps(a.v, div.v);

	return result;
}

lane_f32 operator-(lane_f32 a, f32 div)
{
	lane_f32 result = a - LaneF32FromF32(div);

	return result;
}

lane_f32 operator-(f32 a, lane_f32 div)
{
	lane_f32 result = LaneF32FromF32(a) - div;

	return result;
}

lane_f32 operator-(lane_f32 a)
{
	lane_f32 result = LaneF32FromF32(0) - a;

	return result;
}

lane_f32 operator*(lane_f32 a, lane_f32 div)
{
	lane_f32 result;
	result.v = _mm_mul_ps(a.v, div.v);

	return result;
}

lane_f32 operator*(lane_f32 a, f32 div)
{
	lane_f32 result = a * LaneF32FromF32(div);

	return result;
}

lane_f32 operator*(f32 a, lane_f32 div)
{
	lane_f32 result = LaneF32FromF32(a) * div;

	return result;
}

lane_v3 operator*(lane_v3 a, lane_f32 b)
{
	lane_v3 result;
	result.x = a.x * b;
	result.y = a.y * b;
	result.z = a.z * b;

	return result;
}

lane_v3 operator*(lane_f32 a, lane_v3 b)
{
	lane_v3 result = b * a;

	return result;
}

lane_f32 operator/(lane_f32 a, lane_f32 div)
{
	lane_f32 result;
	result.v = _mm_div_ps(a.v, div.v);

	return result;
}

lane_f32 operator/(lane_f32 a, f32 div)
{
	lane_f32 result = a / LaneF32FromF32(div);

	return result;
}

lane_f32 operator/(f32 a, lane_f32 div)
{
	lane_f32 result = LaneF32FromF32(a) / div;

	return result;
}

lane_v3 operator+(lane_v3 a, lane_v3 b)
{
	lane_v3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;

	return result;
}

lane_f32 SquareRoot(lane_f32 a)
{
	lane_f32 result;
	// may use here the rsqrts instead (more speed but less accurate)
	result.v = _mm_sqrt_ps(a.v);

	return result;
}

void ConditionalAssign(lane_f32* dest, lane_u32 mask, lane_f32 source)
{
	__m128 maskPS = _mm_castsi128_ps(mask.v);
	dest->v = _mm_or_ps(_mm_andnot_ps(maskPS, dest->v), _mm_and_ps(maskPS, source.v));
}

void ConditionalAssign(lane_u32* dest, lane_u32 mask, lane_u32 source)
{
	*dest = AndNot(mask, *dest) | (mask & source);
}

lane_f32 Min(lane_f32 a, lane_f32 b)
{
	lane_f32 result;
	result.v = _mm_min_ps(a.v, b.v);

	return result;
}

lane_f32 Max(lane_f32 a, lane_f32 b)
{
	lane_f32 result;
	result.v = _mm_max_ps(a.v, b.v);

	return result;
}

lane_f32 Clamp01(lane_f32 value)
{
	lane_f32 result = Min(Max(value, LaneF32FromF32(0.0f)), LaneF32FromF32(1.0f));

	return result;
}

lane_f32 GatherF32_(void* basePtr, u32 stride, lane_u32 indices)
{
	u32* v = (u32*)&indices.v;
	lane_f32 result;
	result.v = _mm_setr_ps(*(f32*)((u8*)basePtr + v[0] * stride),
		*(f32*)((u8*)basePtr + v[1] * stride),
		*(f32*)((u8*)basePtr + v[2] * stride),
		*(f32*)((u8*)basePtr + v[3] * stride));

	return result;
}

bool MaskIsZero(lane_u32 mask)
{
	int result = _mm_movemask_epi8(mask.v);

	return (result == 0);
}

u64 HorizontalAdd(lane_u32 a)
{
	u32* v = (u32*)&(a.v);
	u64 result = (u64)v[0] + (u64)v[1] + (u64)v[2] + (u64)v[3];

	return result;
}

f32 HorizontalAdd(lane_f32 a)
{
	f32* v = (f32*)&(a.v);
	f32 result = v[0] + v[1] + v[2] + v[3];

	return result;
}

#endif