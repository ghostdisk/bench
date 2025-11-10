#pragma once
#include <bench/core/common.hpp>
#include <bench/core/refhandle.hpp>

#define BENCHCOROAPI __cdecl

namespace bench {

// TODO: Investigate why printf on Windows XP fails with 8KB coro stack size, but succeeds on Windows 10.
//       I find it hard to believe that the XP version uses *less* stack space.
//       Additionally, the CRT is statically linked so, so the majority of the code executed should be 1:1.
static constexpr int COROUTINE_STACK_SIZE = (16 << 10);

struct Coroutine;

struct Fence {
	U32 value = 0;
};

using CoroutineHandle = RefHandle<Coroutine>;

using CoroutineProc = void (*)(CoroutineHandle coro_handle, void* userdata);

CoroutineHandle CreateCoroutine(void** out_userdata_storage, CoroutineProc** out_proc_location);

template <typename F>
inline CoroutineHandle StartCoroutine(F&& lambda) {
	void* userdata;
	CoroutineProc* fnptrptr;
	CoroutineHandle coro = CreateCoroutine(&userdata, &fnptrptr);
	new (userdata) F(Move(lambda));
	*fnptrptr = [](CoroutineHandle handle, void* userdata) {
		(*((F*)userdata))(handle);
	};
	return coro;
}

bool ExecScheduledCoroutines();


void BlockCoroutine(Coroutine* coro, int blockers_count);

void UnblockCurrentCoroutine(Coroutine* coro, int blockers_count);

bool UnblockCoroutine(Coroutine* coro, int blockers_count);

Fence CreateFence();

void SignalFence(Fence fence);

void WaitForFence(Coroutine* coro, Fence fence);

void Yield(bench::Coroutine* coro);

}
