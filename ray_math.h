#if !defined RAY_MATH_H
# define RAY_MATH_H

inline f32 square(f32 a)
{
	return a * a;
}

inline f32 clamp(f32 min, f32 max, f32 value)
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

inline f32 xlamp01(f32 value)
{
	f32 result = clamp(0.0f, 1.0f, value);

	return result;
}

inline i32 sign_of(i32 a)
{
	i32 result = a >= 0 ? 1 : -1;
	return result;
}

inline f32 square_root(f32 a)
{
	f32 result = sqrtf(a);
	return result;
}

inline f32 safe_ratio_n(f32 numerator, f32 divisor, f32 n)
{
	f32 result = n;

	if (divisor != 0.0f)
	{
		result = numerator / divisor;
	}

	return result;
}

inline f32 safe_ratio_0(f32 numerator, f32 divisor)
{
	return safe_ratio_n(numerator, divisor, 0.0f);
}


inline u32 round_f32_to_u32(f32 f)
{
	u32 result = (u32)(f + 0.5);
	return result;
}

inline i32 round_f32_to_i32(f32 f)
{
	i32 result = (i32)(f + 0.5);
	return result;
}

inline i32 floor_f32_to_u32(f32 f)
{
	i32 result = (i32)floorf(f);
	return result;
}

inline i32 ceil_f32_to_u32(f32 f)
{
	i32 result = (i32)ceilf(f);
	return result;
}

inline f32 sin(f32 angle)
{
	f32 result = sinf(angle);
	return result;
}

inline f32 cos(f32 angle)
{
	f32 result = cosf(angle);
	return result;
}

inline f32 atan2(f32 y, f32 x)
{
	f32 result = atan2f(y, x);
	return result;
}

///
/// vector 2
///

union Vec2
{
	struct
	{
		f32 x, y;
	};
	f32 e[2];
};

inline Vec2 vec2(f32 a)
{
	Vec2 c;

	c.x = a;
	c.y = a;
	return c;
}

inline Vec2 vec2(f32 x, f32 y)
{
	Vec2 c;

	c.x = x;
	c.y = y;
	return c;
}

inline Vec2 operator+(Vec2 a, Vec2 b)
{
	Vec2 c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}

inline Vec2 operator-(Vec2 a, Vec2 b)
{
	Vec2 c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return c;
}

inline Vec2 operator-(Vec2 a)
{
	Vec2 c;

	c.x = -a.x;
	c.y = -a.y;
	return c;
}

inline Vec2 operator*(f32 a, Vec2 b)
{
	Vec2 c;

	c.x = a * b.x;
	c.y = a * b.y;
	return c;
}

inline Vec2 operator*(Vec2 b, f32 a)
{
	Vec2 c;

	c.x = a * b.x;
	c.y = a * b.y;
	return c;
}

inline Vec2& operator*=(Vec2& a, f32 b)
{
	a = b * a;

	return a;
}

inline Vec2& operator+=(Vec2& a, Vec2 b)
{
	a = a + b;

	return a;
}

inline f32 dot(Vec2 a, Vec2 b)
{
	f32 result = a.x * b.x + a.y * b.y;
	return result;
}

inline f32 vec_length_sq(Vec2 a)
{
	f32 result = dot(a, a);
	return result;
}

inline f32 vec_length(Vec2 a)
{
	f32 result = square_root(vec_length_sq(a));
	return result;
}

inline Vec2 hadamard(Vec2 a, Vec2 b)
{
	Vec2 result = { a.x * b.x, a.y * b.y };

	return result;
}

inline Vec2 clamp01(Vec2 value)
{
	Vec2 result;
	result.x = clamp(0.0f, 1.0f, value.x);
	result.y = clamp(0.0f, 1.0f, value.y);

	return result;
}

///
/// vector 3
///

union Vec3
{
	struct
	{
		f32 x, y, z;
	};
	struct
	{
		f32 r, g, b;
	};
	struct
	{
		Vec2 xy;
		f32 z;
	};
	f32 e[3];
};

inline Vec3 vec3(f32 a)
{
	Vec3 c;

	c.x = a;
	c.y = a;
	c.z = a;
	return c;
}

inline Vec3 vec3(f32 x, f32 y, f32 z)
{
	Vec3 c;

	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

inline Vec3 vec3(Vec2 xy, f32 z)
{
	Vec3 c;

	c.x = xy.x;
	c.y = xy.y;
	c.z = z;
	return c;
}

inline Vec3 operator+(Vec3 a, Vec3 b)
{
	Vec3 c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	return c;
}

inline Vec3 operator-(Vec3 a, Vec3 b)
{
	Vec3 c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	return c;
}

inline Vec3 operator-(Vec3 a)
{
	Vec3 c;

	c.x = -a.x;
	c.y = -a.y;
	c.z = -a.z;
	return c;
}

inline Vec3 operator*(f32 a, Vec3 b)
{
	Vec3 c;

	c.x = a * b.x;
	c.y = a * b.y;
	c.z = a * b.z;
	return c;
}

inline Vec3 operator*(Vec3 b, f32 a)
{
	Vec3 c;

	c.x = a * b.x;
	c.y = a * b.y;
	c.z = a * b.z;
	return c;
}

inline Vec3& operator*=(Vec3& a, f32 b)
{
	a = b * a;

	return a;
}

inline Vec3& operator+=(Vec3& a, Vec3 b)
{
	a = a + b;

	return a;
}

inline f32 dot(Vec3 a, Vec3 b)
{
	f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
	return result;
}

inline Vec3 cross(Vec3 a, Vec3 b)
{
	Vec3 result;

	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;

	return result;
}

inline f32 vec_length_sq(Vec3 a)
{
	f32 result = dot(a, a);
	return result;
}

inline f32 vec_length(Vec3 a)
{
	f32 result = square_root(vec_length_sq(a));
	return result;
}

inline Vec3 hadamard(Vec3 a, Vec3 b)
{
	Vec3 result = { a.x * b.x, a.y * b.y, a.z * b.z };

	return result;
}

inline Vec3 clamp01(Vec3 value)
{
	Vec3 result;
	result.x = clamp(0.0f, 1.0f, value.x);
	result.y = clamp(0.0f, 1.0f, value.y);
	result.z = clamp(0.0f, 1.0f, value.z);

	return result;
}

inline Vec3 vec_normalize(Vec3 a)
{
	Vec3 result = {};
	f32 lenSq = vec_length_sq(a);
	if (lenSq > square(0.0001f))
	{
		result = a * (1.0f / square_root(lenSq));
	}

	return result;
}

///
/// vector 4
///

union Vec4
{
	struct
	{
		f32 x, y, z, w;
	};
	struct
	{
		f32 r, g, b, a;
	};
	f32 e[4];
};

inline Vec4 vec4(f32 x, f32 y, f32 z, f32 w)
{
	Vec4 c;

	c.x = x;
	c.y = y;
	c.z = z;
	c.w = w;
	return c;
}

inline Vec4 vec4(Vec3 a, f32 w)
{
	Vec4 c;

	c.x = a.x;
	c.y = a.y;
	c.z = a.z;
	c.w = w;
	return c;
}

inline Vec4 operator+(Vec4 a, Vec4 b)
{
	Vec4 c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	c.w = a.w + b.w;
	return c;
}

inline Vec4 operator-(Vec4 a, Vec4 b)
{
	Vec4 c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	c.w = a.w - b.w;
	return c;
}

inline Vec4 operator-(Vec4 a)
{
	Vec4 c;

	c.x = -a.x;
	c.y = -a.y;
	c.z = -a.z;
	c.w = -a.w;
	return c;
}

inline Vec4 operator*(f32 a, Vec4 b)
{
	Vec4 c;

	c.x = a * b.x;
	c.y = a * b.y;
	c.z = a * b.z;
	c.w = a * b.w;
	return c;
}

inline Vec4 operator*(Vec4 b, f32 a)
{
	Vec4 c;

	c.x = a * b.x;
	c.y = a * b.y;
	c.z = a * b.z;
	c.w = a * b.w;
	return c;
}

inline Vec4& operator*=(Vec4& a, f32 b)
{
	a = b * a;

	return a;
}

inline Vec4& operator+=(Vec4& a, Vec4 b)
{
	a = a + b;

	return a;
}

inline u32 rgba_pack4x8(Vec4 unpacked)
{
	u32 result = ((round_f32_to_u32(unpacked.a) << 24) |
				(round_f32_to_u32(unpacked.b) << 16) |
				(round_f32_to_u32(unpacked.g) << 8) |
				(round_f32_to_u32(unpacked.r) << 0));

	return result;
}

inline u32 bgra_pack4x8(Vec4 unpacked)
{
	u32 result = ((round_f32_to_u32(unpacked.a) << 24) |
				(round_f32_to_u32(unpacked.r) << 16) |
				(round_f32_to_u32(unpacked.g) << 8) |
				(round_f32_to_u32(unpacked.b) << 0));

	return result;
}

inline Vec4 linear1_to_SRGB255(Vec4 c)
{
	Vec4 result;

	f32 one255 = 255.0f;

	result.r = one255 * square_root(c.r);
	result.g = one255 * square_root(c.g);
	result.b = one255 * square_root(c.b);
	result.a = one255 * c.a;

	return result;
}

inline f32 linear_to_SRGB255(f32 l)
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

inline f32 lerp(f32 a, f32 b, f32 t)
{
	f32 result = (1.0f - t) * a + t * b;
	return result;
}

inline Vec3 lerp(Vec3 a, Vec3 b, f32 t)
{
	Vec3 result = (1.0f - t) * a + t * b;
	return result;
}

#endif