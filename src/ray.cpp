#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

#define RAYS_PER_PIXEL 1024
#define USE_MULTI_THREADING 1 // use multi threading
#define USE_SIMD 1 // use SSE2 instructions

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef float f32;
typedef double f64;

#define U32_MAX ((u32) - 1)

#include <math.h>
#include <float.h>
#include "ray_lane.h"
#include "ray_math.h"
#include "ray.h"
#include "random_gen.h"

#include "ray_win32.h"

static u32 GetTotalPixelSize(ImageU32 image)
{
	return sizeof(u32) * image.width * image.height;
}

static ImageU32 CreateImage(u32 width, u32 height)
{
	ImageU32 image = {};
	image.width = width;
	image.height = height;
	u32 outputSize = GetTotalPixelSize(image);
	image.pixels = (u32*)malloc(outputSize);

	return image;
}

static void WriteImage(ImageU32 image, const char* fileName)
{
	u32 outputSize = GetTotalPixelSize(image);
	BitmapHeader header = {};
	header.fileType = 0x4D42;
	header.fileSize = sizeof(header) + outputSize;
	header.bitmapOffset = sizeof(header);
	header.size = sizeof(header) - 14;
	header.width = image.width;
	header.height = image.height;
	header.planes = 1;
	header.bitsPerPixel = 32;
	header.compression = 0;
	header.sizeOfBitmap = outputSize;
	header.hRez = 0;
	header.vRez = 0;
	header.colorsUsed = 0;
	header.colorsImportant = 0;

	FILE* file = fopen(fileName, "wb");
	if (file)
	{
		fwrite(&header, sizeof(header), 1, file);
		fwrite(image.pixels, outputSize, 1, file);
		fclose(file);
	}
	else
	{
		fprintf(stderr, "[ERROR] Unable to write output file %s.\n", fileName);
	}
}

static u32* GetPixelPointer(ImageU32* image, u32 x, u32 y)
{
	u32* result = image->pixels + x + (u64)y * image->width;
	return result;
}

static void CastSampleRays(CastState* cast)
{
	World* world = cast->world;
	u32 raysPerPixel = cast->raysPerPixel;
	u32 maxBounceCount = cast->maxBounceCount;
	lane_f32 filmX = LaneF32FromF32(cast->filmX + cast->halfPixW);
	lane_f32 filmY = LaneF32FromF32(cast->filmY + cast->halfPixH);
	lane_v3 filmCenter = LaneV3FromV3(cast->filmCenter);
	lane_f32 filmW = LaneF32FromF32(cast->filmW);
	lane_f32 filmH = LaneF32FromF32(cast->filmH);
	lane_f32 halfPixW = LaneF32FromF32(cast->halfPixW);
	lane_f32 halfPixH = LaneF32FromF32(cast->halfPixH);
	lane_v3 cameraX = LaneV3FromV3(cast->cameraX);
	lane_v3 cameraY = LaneV3FromV3(cast->cameraY);
	lane_v3 cameraPos = LaneV3FromV3(cast->cameraPos);
	RandomSeries* entropy = cast->entropy;

	lane_u32 bounces = LaneU32FromU32(0);
	lane_v3 color = {};

	u32 laneRayCount = raysPerPixel / LANE_WIDTH;
	assert(laneRayCount * LANE_WIDTH ==	raysPerPixel);

	f32	contrib = 1.0f / (f32)raysPerPixel;
	for (u32 rayIndex = 0; rayIndex < laneRayCount; ++rayIndex)
	{
		lane_f32 offX = filmX + halfPixW * RandomFloatBi(entropy);
		lane_f32 offY = filmY + halfPixH * RandomFloatBi(entropy);
		lane_v3 filmPos = filmCenter + offX * 0.5f * filmW * cameraX + offY * 0.5f * filmH * cameraY;

		lane_v3 rayOrigin = cameraPos;
		lane_v3 rayDir = VecNormalize(filmPos - cameraPos);

		lane_f32 minHitDist = LaneF32FromF32(0.001f);
		//NOTE: temporary epsilon
		lane_f32 epsilon = LaneF32FromF32(0.0001f);

		lane_v3 sample = {};
		lane_v3 attenuation = Vec3(1.0f, 1.0f, 1.0f);

		lane_u32 laneMask = LaneU32FromU32(0xffffffff);

		for (u32 bounce = 0; bounce < maxBounceCount; ++bounce)
		{
			lane_u32 laneIncrement = LaneU32FromU32(1);
			bounces += (laneIncrement & laneMask);

			lane_f32 hitDist = LaneF32FromF32(FLT_MAX);
			lane_u32 hitMaterial = LaneU32FromU32(0);
			lane_v3 nextNormal = {};
			for (u32 planeIndex = 0; planeIndex < world->planeCount; ++planeIndex)
			{
				Plane* plane = &world->planes[planeIndex];

				lane_v3 planeN = LaneV3FromV3(plane->normal);
				lane_f32 planeDist = LaneF32FromF32(plane->dist);

				lane_f32 denom = Dot(planeN, rayDir);
				lane_u32 denomMask = ((denom < -epsilon) | (denom > epsilon));
				if (!MaskIsZero(denomMask))
				{
					lane_f32 t = (-planeDist - Dot(planeN, rayOrigin)) / denom;
					lane_u32 tMask = ((t > minHitDist) & (t < hitDist));
					lane_u32 hitMask = denomMask & tMask;
					if (!MaskIsZero(hitMask))
					{
						lane_u32 planeMatIndex = LaneU32FromU32(plane->matIndex);
						ConditionalAssign(&hitDist, hitMask, t);
						ConditionalAssign(&hitMaterial, hitMask, planeMatIndex);
						ConditionalAssign(&nextNormal, hitMask, planeN);
					}
				}
			}

			for (u32 sphereIndex = 0; sphereIndex < world->sphereCount; ++sphereIndex)
			{
				Sphere* sphere = &world->spheres[sphereIndex];

				lane_v3 spherePos = LaneV3FromV3(sphere->pos);
				lane_f32 sphereRadius = LaneF32FromF32(sphere->radius);

				lane_v3 sphereRelRayOrigin = rayOrigin - spherePos;
				lane_f32 a = Dot(rayDir, rayDir);
				lane_f32 b = 2.0f * Dot(rayDir, sphereRelRayOrigin);
				lane_f32 c = Dot(sphereRelRayOrigin, sphereRelRayOrigin) - sphereRadius * sphereRadius;

				lane_f32 d = SquareRoot(b * b - 4.0f * a * c);

				lane_u32 rootMask = d > epsilon;
				if (!MaskIsZero(rootMask))
				{
					lane_f32 denom = 2.0f * a;
					lane_f32 tp = (-b + d) / denom;
					lane_f32 tn = (-b - d) / denom;

					lane_f32 t = tp;
					lane_u32 pickMask = (tn > minHitDist) & (tn < tp);
					ConditionalAssign(&t, pickMask, tn);

					lane_u32 tMask = (t > minHitDist) & (t < hitDist);
					lane_u32 hitMask = rootMask & tMask;
					if (!MaskIsZero(hitMask))
					{
						lane_u32 sphereMatIndex = LaneU32FromU32(sphere->matIndex);
						ConditionalAssign(&hitDist, hitMask, t);
						ConditionalAssign(&hitMaterial, hitMask, sphereMatIndex);
						ConditionalAssign(&nextNormal, hitMask, VecNormalize(t * rayDir + sphereRelRayOrigin));
					}
				}
			}

			lane_v3 emitColor = laneMask & GATHER_V3(world->materials, hitMaterial, emitColor); // NOTE: must return 0 on laneMask
			lane_v3 reflectColor = GATHER_V3(world->materials, hitMaterial, reflectColor);
			lane_f32 matSpecular = GATHER_F32(world->materials, hitMaterial, specular);

			sample += Hadamard(attenuation, emitColor);
			laneMask &= (hitMaterial != LaneU32FromU32(0)); // NOTE: disable the dead ray

			if (MaskIsZero(laneMask)) // NOTE: all rays are dead
			{
				break;
			}

			lane_f32 cosAtten = Max(Dot(-rayDir, nextNormal), LaneF32FromF32(0.0f));
			attenuation = Hadamard(attenuation, cosAtten * reflectColor);

			rayOrigin += hitDist * rayDir;
			// TODO: reflection
			lane_v3 reflectedRay = rayDir - 2 * Dot(rayDir, nextNormal) * nextNormal;
			lane_v3 randomBounce = VecNormalize(nextNormal + LaneV3(RandomFloatBi(entropy), RandomFloatBi(entropy), RandomFloatBi(entropy)));
			rayDir = VecNormalize(Lerp(randomBounce, reflectedRay, matSpecular));
		}

		color += contrib * sample;
	}

	cast->bouncesComputed += HorizontalAdd(bounces);
	cast->finalColor = HorizontalAdd(color);
	cast->entropy = entropy;
}

static bool RenderTile(WorkQueue* queue)
{
	u64 workOrderIndex = LockedAdd(&queue->NextWorkOrderIndex, 1);
	if (workOrderIndex >= queue->workOrderCount)
	{
		return false;
	}

	WorkOrder* order = &queue->workOrders[workOrderIndex];

	ImageU32* image = &order->image;

	u32 xMin = order->minX;
	u32 xMax = order->maxX;
	u32 yMin = order->minY;
	u32 yMax = order->maxY;
	f32 filmDist = 1.0f;

	lane_v3 cameraPos = Vec3(0, -10, 1);
	lane_v3 cameraZ = VecNormalize(cameraPos);
	lane_v3 cameraX = VecNormalize(Cross(Vec3(0, 0, 1), cameraZ));
	lane_v3 cameraY = VecNormalize(Cross(cameraZ, cameraX));
	lane_v3 filmCenter = cameraPos - filmDist * cameraZ;

	CastState castState;

	castState.world = order->world;
	castState.raysPerPixel = queue->raysPerPixel;
	castState.maxBounceCount = queue->maxBounceCount;
	castState.entropy = &order->entropy;

	castState.cameraPos = Extract0(cameraPos);
	castState.cameraZ = Extract0(cameraZ);
	castState.cameraX = Extract0(cameraX);
	castState.cameraY = Extract0(cameraY);

	castState.filmW = 1.0f;
	castState.filmH = 1.0f;
	if (image->width > image->height)
	{
		castState.filmH = castState.filmW * ((f32)image->height / (f32)image->width);
	}
	else if (image->height > image->width)
	{
		castState.filmW = castState.filmH * ((f32)image->width / (f32)image->height);
	}
	castState.filmCenter = Extract0(filmCenter);

	castState.halfPixW = 0.5f / image->width;
	castState.halfPixH = 0.5f / image->height;

	castState.bouncesComputed = 0;
	for (u32 y = yMin; y < yMax; ++y)
	{
		u32* out =  GetPixelPointer(image, xMin, y);

		castState.filmY = -1.0f + 2.0f * ((f32)y / (f32)image->height);
		for (u32 x = xMin; x < xMax; ++x)
		{
			castState.filmX = -1.0f + 2.0f * ((f32)x / (f32)image->width);
			
			CastSampleRays(&castState);

			// TODO: real sRGB
			f32 r = 255.0f * LinearToSRGB255(castState.finalColor.x);
			f32 g = 255.0f * LinearToSRGB255(castState.finalColor.y);
			f32 b = 255.0f * LinearToSRGB255(castState.finalColor.z);
			f32 a = 255.0f;

			u32 bmpValue = ((RoundF32ToU32(a) << 24) |
							(RoundF32ToU32(r) << 16) |
							(RoundF32ToU32(g) << 8) |
							(RoundF32ToU32(b) << 0));
			*out++ = bmpValue; // y < 32 ? 0xFF00CCEE : 0xFF6600EE;
		}
	}

	LockedAdd(&queue->totalBounces, castState.bouncesComputed);
	LockedAdd(&queue->tileCount, 1);

	return true;
}

int main(int argc, char** argv)
{
	Material materials[7] =
	{
		//{ {0.5f, 0.8f, 1.0f}, { }, 0.0f },
		{ {0.01f, 0.01f, 0.01f}, { }, 0.0f },
		{ { }, {0.6f, 0.6f, 0.6f}, 0.0f },
		{ { }, {0.5f, 0.5f, 1.0f}, 0.0f },
		{ {40.0f, 10.0f, 1.0f}, { }, 0.0f },
		{ { }, {0.1f, 1.0f, 0.8f}, 1.0f },
		{ { }, {0.5f, 0.2f, 0.9f}, 0.85f },
		{ { }, {0.99f, 0.99f, 0.99f}, 1.0f },
	};

	Plane planes[] =
	{
		{{0, 0, 1}, 0, 1 },
	};

	Sphere spheres[] =
	{
		{{0.0f, -1.0f, 0.0f}, 1.0f, 2},
		{{3.0f, -2.0f, 0.0f}, 1.0f, 3},
		{{-2.0f, -1.0f, 2.0f}, 1.0f, 4},
		{{1.0f, -1.0f, 2.5f}, 1.0f, 5},
		{{-3.0f, 5.0f, 0.0f}, 3.0f, 6},
	};

	World world = {};
	world.materialCount = ARRAY_COUNT(materials);
	world.materials = materials;
	world.planeCount = ARRAY_COUNT(planes);
	world.planes = planes;
	world.sphereCount = ARRAY_COUNT(spheres);
	world.spheres = spheres;

	ImageU32 image = CreateImage(1920, 1080);

	u32 coreCount = 1;
#if USE_MULTI_THREADING
	coreCount = GetCpuCoreCount();
#endif
	u32 tileW = image.width / coreCount;
	u32 tileH = tileW;
	tileH = tileW = 64;
	u32 tileCountX = (image.width + tileW - 1) / tileW;
	u32 tileCountY = (image.height + tileH - 1) / tileH;
	u32 tileTotal = tileCountX * tileCountY;

	WorkQueue queue = {};
	queue.workOrders = (WorkOrder *)malloc(tileTotal * sizeof(WorkOrder));
	queue.raysPerPixel = RAYS_PER_PIXEL;
	queue.maxBounceCount = 8;

	printf("Config: %d cores with %d of %dx%d tiles, with %d-wide lanes\n", coreCount, tileTotal, tileW, tileH, LANE_WIDTH);
	printf("Quality: %d rays per pixel, max %d bounces\n", queue.raysPerPixel, queue.maxBounceCount);

	for (u32 tileY = 0; tileY < tileCountY; ++tileY)
	{
		u32 minY = tileY * tileH;
		u32 maxY = minY + tileH;
		if (maxY > image.height)
		{
			maxY = image.height;
		}

		for (u32 tileX = 0; tileX < tileCountX; ++tileX)
		{
			u32 minX = tileX * tileW;
			u32 maxX = minX + tileW;
			if (maxX > image.width)
			{
				maxX = image.width;
			}

			WorkOrder* order = &queue.workOrders[queue.workOrderCount++];
			assert(queue.workOrderCount <= tileTotal);

			order->world = &world;
			order->image = image;
			order->minX = minX;
			order->maxX = maxX;
			order->minY = minY;
			order->maxY = maxY;

			// NOTE: temporary entropy
			RandomSeries entropy = {LaneU32FromU32(29807612 + tileX * 12301 + tileY * 127659,
												56094376 + tileX * 31085 + tileY * 805672,
												10957868 + tileX * 29067 + tileY * 310784,
												67820193 + tileX * 17453 + tileY * 209485) };
			order->entropy = entropy;
		}
	}
	assert(queue.workOrderCount == tileTotal);

	// NOTE: For fencing
	LockedAdd(&queue.NextWorkOrderIndex, 0);

	clock_t startClock = clock();

	for (u32 coreIndex = 1; coreIndex < coreCount; ++coreIndex)
	{
		CreateThread(&queue);
	}

	while (queue.tileCount < tileTotal)
	{
		if (RenderTile(&queue))
		{
			printf("\rRaycasting %d%%...   ", 100 * (u32)queue.tileCount / tileTotal);
			fflush(stdout);
		}
	}

	clock_t endClock = clock();
	clock_t elapsed = endClock - startClock;
	printf("\nRaycasting Time: %d ms\n", elapsed);
	printf("Total bounces: %llu\n", queue.totalBounces);
	printf("Performance %f ms/bounce\n", elapsed / (f64)queue.totalBounces);

	WriteImage(image, "result.bmp");
	printf("Done!\n");
	return 0;
}