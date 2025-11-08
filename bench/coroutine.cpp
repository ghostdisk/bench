#pragma once
#include <bench/coroutine.hpp>
#include <bench/sys.hpp>
#include <bench/utils/arraylist.hpp>
#include <stdlib.h>
#include <atomic>

namespace bench {

extern "C" bool __cdecl bench_Resume_CoroCompleted(bench::Coroutine* coro);

struct Coroutine {
	U32* stack_low;
	U32* stack_high;
	U32 coro_esp;
	U32 orig_esp;
	std::atomic<int> refcount;
	std::atomic<int> blockers_count;
};

static ArrayList<CoroutineHandle> g_scheduled_coroutines;

template <>
void AddRef(Coroutine* coro) {
	++coro->refcount;
}

template <>
void RemoveRef(Coroutine* coro) {
	if (--coro->refcount == 0) {
		VirtualFree(coro->stack_low, COROUTINE_STACK_SIZE_BYTES, VirtualFreeType::RELEASE);
		delete coro;
	}
}

CoroutineHandle StartCoroutine(void (BENCHCOROAPI *entry)(CoroutineHandle coro, void* userdata), void* userdata) {
	Coroutine* coro = new Coroutine();
	coro->stack_low = (U32*)VirtualAlloc(nullptr, COROUTINE_STACK_SIZE_BYTES, VirtualAllocType::COMMIT, VirtualMemoryProtection::READ | VirtualMemoryProtection::WRITE);
	coro->stack_high = coro->stack_low + COROUTINE_STACK_SIZE_DWORDS;
	AssertAlways(coro->stack_low, "Out of memory");

#ifdef BENCH_DEBUG
	memset(coro->stack_low, 0x14, COROUTINE_STACK_SIZE_BYTES);
#endif

	// ------------------------------------------------------------
	// Setup stack:

	coro->stack_low[0] = 0x13371337; // stack overflow guard

	*(coro->stack_high - 1) = (U32)userdata; // entry arg2
	*(coro->stack_high - 2) = (U32)coro; // entry arg1
	*(coro->stack_high - 3) = (U32)bench_Resume_CoroCompleted; // ret addr when coro completes
	*(coro->stack_high - 4) = (U32)entry; // ret addr for the first Resume()
	*(coro->stack_high - 5) = (U32)coro; // value of EBP after coro completes
	// *(coro->stack_hgih - 6) = 0; // EDI
	// *(coro->stack_hgih - 7) = 0; // ESI
	// *(coro->stack_hgih - 8) = 0; // EBX 
	coro->coro_esp = (U32)(coro->stack_high - 8);

	// entry arg1 is of type CoroutineHandle and will decrement refcount when the coroutine completes,
	// but it was never constructed, so we have to manually increment refcount:
	coro->refcount++;

	g_scheduled_coroutines.Push(coro);
	return coro;
}

static void CheckStackSmash(Coroutine* coro) {
	AssertAlways(coro->stack_low[0] == 0x13371337, "Coroutine stack overflow");
}

bool ExecScheduledCoroutines() {
	ArrayList<CoroutineHandle> coroutines = Move(g_scheduled_coroutines);

	// TODO!!! If we do `const CoroutineHandle&` here, MSVC generates broken code on /O2.
	for (const CoroutineHandle& handle : coroutines) {
		ResumeCoroutine(handle);
	}

	return coroutines.m_size > 0;
}

void BlockCoroutine(Coroutine* coro, int blockers_count) {
	coro->blockers_count += blockers_count;
}

bool UnblockCoroutine(Coroutine* coro, int blockers_count) {
	if ((coro->blockers_count -= blockers_count) == 0) {
		g_scheduled_coroutines.Emplace(coro);
		return true;
	}
	return false;
}

}
