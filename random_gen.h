#if !defined RANDOM_GEN
# define RANDOM_GEN

//
// Random generation
//

// NOTE: based on article PCG: A Family of Simple Fast Space - Efficient Statistically Good Algorithms for Random Number Generation, by Melissa E. O’Neill
static lane_u32 XORshift32(RandomSeries* series)
{
	lane_u32 x = series->state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	series->state = x;

	return x;
}

static lane_f32 RandomFloatUni(RandomSeries* series)
{
	// NOTE: shift the sign bit for proper work of _mm_cvtepi32_ps
	lane_f32 result = LaneF32FromU32(XORshift32(series) >> 1) / (f32)(U32_MAX >> 1);
	return result;
}

static lane_f32 RandomFloatBi(RandomSeries* series)
{
	lane_f32 result = -1.0f + 2.0f * RandomFloatUni(series);
	return result;
}

//static lane_f32 RandomLaneBi(RandomSeries* series)
//{
//	lane_f32 result = RandomFloatBi(series);
//	return result;
//}

#endif