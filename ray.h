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
	Vec3 emitColor;
	Vec3 reflectColor;
	f32 specular; // 0 - pure diffuse, 1 - mirror
};

struct Plane
{
	Vec3 normal;
	f32 dist;
	u32 matIndex;
};

struct Sphere
{
	Vec3 pos;
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

struct WorkOrder
{
	World* world;
	ImageU32 image;
	u32 minX;
	u32 maxX;
	u32 minY;
	u32 maxY;
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

#endif