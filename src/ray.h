#if !defined RAY_H
# define RAY_H

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#pragma pack(push, 1)
struct BitmapHeader
{
	u16 fileType;
	u32 fileSize;
	u16 reserved1;
	u16 reserved2;
	u32 bitmapOffset;
	u32 size;
	i32 width;
	i32 height;
	u16 planes;
	u16 bitsPerPixel;
	u32 compression;
	u32 sizeOfBitmap;
	i32 hRez;
	i32 vRez;
	u32 colorsUsed;
	u32 colorsImportant;
};
#pragma pack(pop)

struct ImageU32
{
	u32 width;
	u32 height;
	u32* pixels;
};

struct Material
{
	vec3 emitColor;
	vec3 reflectColor;
	f32 specular; // 0 - pure diffuse, 1 - mirror
};

struct Plane
{
	vec3 normal;
	f32 dist;
	u32 matIndex;
};

struct Sphere
{
	vec3 pos;
	f32 radius;
	u32 matIndex;
};

struct World
{
	u32 materialCount;
	Material* materials;

	u32 planeCount;
	Plane* planes;

	u32 sphereCount;
	Sphere* spheres;
};

struct RandomSeries
{
	lane_u32 state;
};

struct WorkOrder
{
	World* world;
	ImageU32 image;
	u32 minX;
	u32 maxX;
	u32 minY;
	u32 maxY;
	RandomSeries entropy;
};

struct WorkQueue
{
	u32 workOrderCount;
	WorkOrder* workOrders;
	volatile u64 NextWorkOrderIndex;
	volatile u64 totalBounces;
	volatile u64 tileCount;

	u32 raysPerPixel;
	u32 maxBounceCount;
};


struct CastState
{
	// In
	World* world;
	u32 raysPerPixel;
	u32 maxBounceCount;
	RandomSeries* entropy;

	vec3 cameraX;
	vec3 cameraY;
	vec3 cameraZ;
	vec3 cameraPos;

	f32 filmW;
	f32 filmH;
	vec3 filmCenter;
	f32 halfPixW;
	f32 halfPixH;

	f32 filmX;
	f32 filmY;

	// Out
	vec3 finalColor;
	u64 bouncesComputed;
};

#endif