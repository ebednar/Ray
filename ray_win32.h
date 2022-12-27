#if !defined RAY_WIN32
# define RAY_WIN32

#include <windows.h>

static bool render_tile(WorkQueue* queue);

static DWORD WINAPI thread_proc(void* lpParameter)
{
	WorkQueue* queue = (WorkQueue*)lpParameter;
	while (render_tile(queue)) {};
	return 0;
}

static void create_thread(void* parametr)
{
	DWORD threadID;
	HANDLE handle = CreateThread(NULL, 0, thread_proc, parametr, 0, &threadID);
	CloseHandle(handle);
}

static u32 get_CPU_core_count()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	u32 result = info.dwNumberOfProcessors;

	return result;
}

static u64 locked_add_and_return_previous(u64 volatile* value, u64 a)
{
	u64 result = InterlockedExchangeAdd64((volatile LONG64*)value, a);

	return result;
}

#endif