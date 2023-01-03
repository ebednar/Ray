#if !defined RAY_MATH_H
# define RAY_MATH_H

inline f32 Square(f32 a)
{
	return a * a;
}

inline f32 Clamp(f32 min, f32 max, f32 value)
{
	f32 result = value;

	if (result < min)
	{
		result = min;
	}
	else if (result > max)
	{
		result = max;
	}

	return result;
}

inline f32 Clamp01(f32 value)
{
	f32 result = Clamp(0.0f, 1.0f, value);

	return result;
}

inline i32 SignOf(i32 a)
{
	i32 result = a >= 0 ? 1 : -1;
	return result;
}

inline f32 SquareRoot(f32 a)
{
	f32 result = sqrtf(a);
	return result;
}

inline f32 SafeRatioN(f32 numerator, f32 divisor, f32 n)
{
	f32 result = n;

	if (divisor != 0.0f)
	{
		result = numerator / divisor;
	}

	return result;
}

inline f32 SafeRatio0(f32 numerator, f32 divisor)
{
	return SafeRatioN(numerator, divisor, 0.0f);
}


inline u32 RoundF32ToU32(f32 f)
{
	u32 result = (u32)(f + 0.5);
	return result;
}

inline i32 RoundF32ToI32(f32 f)
{
	i32 result = (i32)(f + 0.5);
	return result;
}

inline i32 FloorF32ToU32(f32 f)
{
	i32 result = (i32)floorf(f);
	return result;
}

inline i32 CeilF32ToU32(f32 f)
{
	i32 result = (i32)ceilf(f);
	return result;
}

inline f32 Sin(f32 angle)
{
	f32 result = sinf(angle);
	return result;
}

inline f32 Cos(f32 angle)
{
	f32 result = cosf(angle);
	return result;
}

inline f32 Atan2(f32 y, f32 x)
{
	f32 result = atan2f(y, x);
	return result;
}

///
/// vector 2
///

union vec2
{
	struct
	{
		f32 x, y;
	};
	f32 e[2];
};

inline vec2 Vec2(f32 a)
{
	vec2 c;

	c.x = a;
	c.y = a;
	return c;
}

inline vec2 Vec2(f32 x, f32 y)
{
	vec2 c;

	c.x = x;
	c.y = y;
	return c;
}

inline vec2 operator+(vec2 a, vec2 b)
{
	vec2 c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}

inline vec2 operator-(vec2 a, vec2 b)
{
	vec2 c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return c;
}

inline vec2 operator-(vec2 a)
{
	vec2 c;

	c.x = -a.x;
	c.y = -a.y;
	return c;
}

inline vec2 operator*(f32 a, vec2 b)
{
	vec2 c;

	c.x = a * b.x;
	c.y = a * b.y;
	return c;
}

inline vec2 operator*(vec2 b, f32 a)
{
	vec2 c;

	c.x = a * b.x;
	c.y = a * b.y;
	return c;
}

inline vec2& operator*=(vec2& a, f32 b)
{
	a = b * a;

	return a;
}

inline vec2& operator+=(vec2& a, vec2 b)
{
	a = a + b;

	return a;
}

inline f32 Dot(vec2 a, vec2 b)
{
	f32 result = a.x * b.x + a.y * b.y;
	return result;
}

inline f32 VecLengthSq(vec2 a)
{
	f32 result = Dot(a, a);
	return result;
}

inline f32 VecLength(vec2 a)
{
	f32 result = SquareRoot(VecLengthSq(a));
	return result;
}

inline vec2 Hadamard(vec2 a, vec2 b)
{
	vec2 result = { a.x * b.x, a.y * b.y };

	return result;
}

inline vec2 Clamp01(vec2 value)
{
	vec2 result;
	result.x = Clamp(0.0f, 1.0f, value.x);
	result.y = Clamp(0.0f, 1.0f, value.y);

	return result;
}

///
/// vector 3
///

//union vec3
//{
//	struct
//	{
//		f32 x, y, z;
//	};
//	struct
//	{
//		f32 r, g, b;
//	};
//	struct
//	{
//		vec2 xy;
//		f32 z;
//	};
//	f32 e[3];
//};

inline lane_v3 Vec3(f32 a)
{
	lane_v3 c;

	c.x = a;
	c.y = a;
	c.z = a;
	return c;
}

inline lane_v3 Vec3(f32 x, f32 y, f32 z)
{
	lane_v3 c;

	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

inline lane_v3 Vec3(vec2 xy, f32 z)
{
	lane_v3 c;

	c.x = xy.x;
	c.y = xy.y;
	c.z = z;
	return c;
}

inline lane_v3 LaneV3(lane_f32 x, lane_f32 y, lane_f32 z)
{
	lane_v3 c;

	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

inline lane_v3 operator-(lane_v3 a, lane_v3 b)
{
	lane_v3 c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	return c;
}

inline lane_v3 operator-(lane_v3 a)
{
	lane_v3 c;

	c.x = -a.x;
	c.y = -a.y;
	c.z = -a.z;
	return c;
}

inline lane_v3 operator*(f32 a, lane_v3 b)
{
	lane_v3 c;

	c.x = a * b.x;
	c.y = a * b.y;
	c.z = a * b.z;
	return c;
}

inline lane_v3 operator*(lane_v3 b, f32 a)
{
	lane_v3 c;

	c.x = a * b.x;
	c.y = a * b.y;
	c.z = a * b.z;
	return c;
}

inline lane_v3& operator*=(lane_v3& a, f32 b)
{
	a = b * a;

	return a;
}

inline lane_v3& operator+=(lane_v3& a, lane_v3 b)
{
	a = a + b;

	return a;
}

inline lane_f32 Dot(lane_v3 a, lane_v3 b)
{
	lane_f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
	return result;
}

inline lane_v3 Cross(lane_v3 a, lane_v3 b)
{
	lane_v3 result;

	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;

	return result;
}

inline lane_f32 VecLengthSq(lane_v3 a)
{
	lane_f32 result = Dot(a, a);
	return result;
}

inline lane_f32 VecLength(lane_v3 a)
{
	lane_f32 result = SquareRoot(VecLengthSq(a));
	return result;
}

inline lane_v3 Hadamard(lane_v3 a, lane_v3 b)
{
	lane_v3 result = { a.x * b.x, a.y * b.y, a.z * b.z };

	return result;
}

inline lane_v3 Clamp01(lane_v3 value)
{
	lane_v3 result;
	result.x = Clamp01(value.x);
	result.y = Clamp01(value.y);
	result.z = Clamp01(value.z);

	return result;
}

inline lane_v3 VecNormalize(lane_v3 a)
{
	lane_v3 result = {};
	lane_f32 lenSq = VecLengthSq(a);
	lane_u32 mask = (lenSq > Square(0.0001f));
	ConditionalAssign(&result, mask, a * (1.0f / SquareRoot(lenSq)));

	return result;
}

inline lane_v3 Lerp(lane_v3 a, lane_v3 b, lane_f32 t)
{
	lane_v3 result = (1.0f - t) * a + t * b;
	return result;
}

inline f32 LinearToSRGB255(f32 l)
{
	f32 s;

	if (l < 0.0f)
	{
		l = 0.0f;
	}
	if (l > 1.0f)
	{
		l = 1.0f;
	}

	s = l * 12.92f;
	if (l > 0.0031308f)
	{
		s = 1.055f * (f32)pow(l, 1.0f / 2.4f) - 0.055f;
	}

	return s;
}

inline f32 Lerp(f32 a, f32 b, f32 t)
{
	f32 result = (1.0f - t) * a + t * b;
	return result;
}

#endif