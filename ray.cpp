#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

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
#include "ray_math.h"
#include "ray.h"

#include "ray_win32.h"

// Speed measurements
// Initial timing: 32 rays - 20400-20650ms, 0.000340 ms/bounce
// With multi threading: 32 rays - 4000-4700ms, 0.000066-0.000079 ms/bounce
// Custom random numbers: 32 rays - 3500-4300ms, 0.000061-0.000072 ms/bounce

static u32 get_total_pixel_size(ImageU32 image)
{
	return sizeof(u32) * image.width * image.height;
}

static ImageU32 create_image(u32 width, u32 height)
{
	ImageU32 image = {};
	image.width = width;
	image.height = height;
	u32 outputSize = get_total_pixel_size(image);
	image.pixels = (u32*)malloc(outputSize);

	return image;
}

static void write_image(ImageU32 image, const char* fileName)
{
	u32 outputSize = get_total_pixel_size(image);
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

static u32 xor_shift32(RandomSeries* series)
{
	u32 x = series->state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	series->state = x;

	return x;
}

static f32 random_float_uni(RandomSeries* series)
{
	f32 result = (f32)xor_shift32(series) / (f32)U32_MAX;
	return result;
}

static f32 random_float_bi(RandomSeries* series)
{
	f32 result = -1.0f + 2.0f * random_float_uni(series);
	return result;
}

static u32* get_pixel_pointer(ImageU32* image, u32 x, u32 y)
{
	u32* result = image->pixels + x + (u64)y * image->width;
	return result;
}

static bool render_tile(WorkQueue* queue)
{
	u64 workOrderIndex = locked_add_and_return_previous(&queue->NextWorkOrderIndex, 1);
	if (workOrderIndex >= queue->workOrderCount)
	{
		return false;
	}

	WorkOrder* order = &queue->workOrders[workOrderIndex];
	World* world = order->world;

	RandomSeries series = order->entropy;

	ImageU32* image = &order->image;

	u32 xMin = order->minX;
	u32 xMax = order->maxX;
	u32 yMin = order->minY;
	u32 yMax = order->maxY;

	Vec3 cameraPos = vec3(0, -10, 1);
	Vec3 cameraZ = vec_normalize(cameraPos);
	Vec3 cameraX = vec_normalize(cross(vec3(0, 0, 1), cameraZ));
	Vec3 cameraY = vec_normalize(cross(cameraZ, cameraX));

	f32 filmDist = 1.0f;
	f32 filmW = 1.0f;
	f32 filmH = 1.0f;
	if (image->width > image->height)
	{
		filmH = filmW * ((f32)image->height / (f32)image->width);
	}
	else if (image->height > image->width)
	{
		filmW = filmH * ((f32)image->width / (f32)image->height);
	}
	Vec3 filmCenter = cameraPos - filmDist * cameraZ;

	f32 halfPixW = 0.5f / image->width;
	f32 halfPixH = 0.5f / image->height;

	u32 raysPerPixel = queue->raysPerPixel;
	u32 maxBounceCount = queue->maxBounceCount;
	for (u32 y = yMin; y < yMax; ++y)
	{
		u32* out =  get_pixel_pointer(image, xMin, y);

		f32 filmY = -1.0f + 2.0f * ((f32)y / (f32)image->height);
		for (u32 x = xMin; x < xMax; ++x)
		{
			f32 filmX = -1.0f + 2.0f * ((f32)x / (f32)image->width);

			f32	contrib = 1.0f / (f32)raysPerPixel;
			Vec3 color = {};
			for (u32 rayIndex = 0; rayIndex < raysPerPixel; ++rayIndex)
			{
				f32 offX = filmX + halfPixW * random_float_bi(&series);
				f32 offY = filmY + halfPixH * random_float_bi(&series);
				Vec3 filmPos = filmCenter + offX * 0.5f * filmW * cameraX + offY * 0.5f * filmH * cameraY;

				Vec3 rayOrigin = cameraPos;
				Vec3 rayDir = vec_normalize(filmPos - cameraPos);

				f32 minHitDist = 0.001f;
				//NOTE: temporary epsilon
				f32 epsilon = 0.0001f;
				u64 bounces = 0;

				Vec3 sample = {};
				Vec3 attenuation = vec3(1.0f, 1.0f, 1.0f);
				for (u32 bounce = 0; bounce < maxBounceCount; ++bounce)
				{
					++bounces;

					f32 hitDist = FLT_MAX;
					u32 hitMaterial = 0;
					Vec3 nextNormal = {};
					for (u32 planeIndex = 0; planeIndex < world->planeCount; ++planeIndex)
					{
						Plane* plane = &world->planes[planeIndex];

						f32 denom = dot(plane->normal, rayDir);
						if (denom < -epsilon || denom > epsilon)
						{
							f32 t = (-plane->dist - dot(plane->normal, rayOrigin)) / denom;
							if (t > minHitDist && t < hitDist)
							{
								hitDist = t;
								hitMaterial = plane->matIndex;

								nextNormal = plane->normal;
							}
						}
					}

					for (u32 sphereIndex = 0; sphereIndex < world->sphereCount; ++sphereIndex)
					{
						Sphere* sphere = &world->spheres[sphereIndex];

						Vec3 sphereRelRayOrigin = rayOrigin - sphere->pos;
						f32 a = dot(rayDir, rayDir);
						f32 b = 2.0f * dot(rayDir, sphereRelRayOrigin);
						f32 c = dot(sphereRelRayOrigin, sphereRelRayOrigin) - sphere->radius * sphere->radius;

						f32 d = square_root(b * b - 4.0f * a * c);
						f32 denom = 2.0f * a;
						if (d > epsilon)
						{
							f32 tp = (-b + d) / denom;
							f32 tn = (-b - d) / denom;

							f32 t = tp;
							if (tn > minHitDist && tn < tp)
							{
								t = tn;
							}

							if (t > minHitDist && t < hitDist)
							{
								hitDist = t;
								hitMaterial = sphere->matIndex;

								nextNormal = vec_normalize(t * rayDir + sphereRelRayOrigin);
							}
						}
					}

					if (hitMaterial)
					{
						Material material = world->materials[hitMaterial];

						sample += hadamard(attenuation, material.emitColor);


						// TODO: cosine reflection
						f32 cosAtten = dot(-rayDir, nextNormal);
						if (cosAtten < 0)
						{
							cosAtten = 0;
						}

						attenuation = hadamard(attenuation, cosAtten * material.reflectColor);

						rayOrigin += hitDist * rayDir;
						// TODO: reflection
						Vec3 reflectedRay = rayDir - 2 * dot(rayDir, nextNormal) * nextNormal;
						Vec3 randomBounce = vec_normalize(nextNormal + vec3(random_float_bi(&series), random_float_bi(&series), random_float_bi(&series)));
						rayDir = vec_normalize(lerp(randomBounce, reflectedRay, material.specular));
					}
					else
					{
						Material material = world->materials[hitMaterial];
						sample += hadamard(attenuation, material.emitColor);
						break;
					}
				}

				locked_add_and_return_previous(&queue->totalBounces, bounces);
				color += contrib * sample;
			}


			// TODO: real sRGB
			Vec4 bmpColor = {
				255.0f * linear_to_SRGB255(color.r),
				255.0f * linear_to_SRGB255(color.g),
				255.0f * linear_to_SRGB255(color.b),
				255.0f
			};
			u32 bmpValue = bgra_pack4x8(bmpColor);

			*out++ = bmpValue; // y < 32 ? 0xFF00CCEE : 0xFF6600EE;
		}
	}

	locked_add_and_return_previous(&queue->tileCount, 1);

	return true;
}

int main(int argc, char** argv)
{
	Material materials[7] = {};
	materials[0].emitColor = vec3(0.5f, 0.8f, 1.0f);
	materials[1].reflectColor = vec3(0.6f, 0.6f, 0.6f);
	materials[2].reflectColor = vec3(0.5f, 0.5f, 1.0f);
	materials[3].emitColor = vec3(8.0f, 4.0f, 0.5f);
	materials[4].reflectColor = vec3(0.1f, 1.0f, 0.8f);
	materials[4].specular = 1.0f;
	materials[5].reflectColor = vec3(0.5f, 0.2f, 0.9f);
	materials[5].specular = 0.85f;
	materials[6].reflectColor = vec3(0.99f, 0.99f, 0.99f);
	materials[6].specular = 1.0f;

	Plane planes[1] = {};
	planes[0].normal = vec3(0, 0, 1);
	planes[0].dist = 0;
	planes[0].matIndex = 1;

	Sphere spheres[5] = {};
	spheres[0].pos = vec3(0.0f, -1.0f, 0.0f);
	spheres[0].radius = 1.0f;
	spheres[0].matIndex = 2;
	spheres[1].pos = vec3(3.0f, -2.0f, 0.0f);
	spheres[1].radius = 1.0f;
	spheres[1].matIndex = 3;
	spheres[2].pos = vec3(-2.0f, -1.0f, 2.0f);
	spheres[2].radius = 1.0f;
	spheres[2].matIndex = 4;
	spheres[3].pos = vec3(1.0f, -1.0f, 2.5f);
	spheres[3].radius = 1.0f;
	spheres[3].matIndex = 5;
	spheres[4].pos = vec3(-3.0f, 5.0f, 0.0f);
	spheres[4].radius = 3.0f;
	spheres[4].matIndex = 6;

	World world = {};
	world.materialCount = ARRAY_COUNT(materials);
	world.materials = materials;
	world.planeCount = ARRAY_COUNT(planes);
	world.planes = planes;
	world.sphereCount = ARRAY_COUNT(spheres);
	world.spheres = spheres;

	ImageU32 image = create_image(1280, 720);

	u32 threadCount = get_CPU_core_count();
	u32 tileW = image.width / threadCount;
	u32 tileH = tileW;
	tileH = tileW = 64;
	u32 tileCountX = (image.width + tileW - 1) / tileW;
	u32 tileCountY = (image.height + tileH - 1) / tileH;
	u32 tileTotal = tileCountX * tileCountY;

	WorkQueue queue = {};
	queue.workOrders = (WorkOrder *)malloc(tileTotal * sizeof(WorkOrder));
	queue.raysPerPixel = 32;
	queue.maxBounceCount = 8;

	printf("Config: %d threads with %d of %dx%d tiles\n", threadCount, tileTotal, tileW, tileH);
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
			RandomSeries series = { tileX * 123 + tileY * 127659 };
			order->entropy = series;
		}
	}
	assert(queue.workOrderCount == tileTotal);

	// NOTE: For fencing
	locked_add_and_return_previous(&queue.NextWorkOrderIndex, 0);

	clock_t startClock = clock();

	for (u32 threadIndex = 1; threadIndex < threadCount; ++threadIndex)
	{
		create_thread(&queue);
	}

	while (queue.tileCount < tileTotal)
	{
		if (render_tile(&queue))
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

	write_image(image, "result.bmp");
	printf("Done!\n");
	return 0;
}