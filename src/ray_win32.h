#if !defined RAY_WIN32
# define RAY_WIN32

#include <windows.h>

static bool RenderTile(WorkQueue* queue);

static DWORD WINAPI ThreadProc(void* lpParameter)
{
	WorkQueue* queue = (WorkQueue*)lpParameter;
	while (RenderTile(queue)) {};
	return 0;
}

static void CreateThread(void* parametr)
{
	DWORD threadID;
	HANDLE handle = CreateThread(NULL, 0, ThreadProc, parametr, 0, &threadID);
	CloseHandle(handle);
}

static u32 GetCpuCoreCount()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	u32 result = info.dwNumberOfProcessors;

	return result;
}

static u64 LockedAdd(u64 volatile* value, u64 a)
{
	u64 result = InterlockedExchangeAdd64((volatile LONG64*)value, a);

	return result;
}

#endif